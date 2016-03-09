/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "smtptask.h"
#include <QDebug>
#include <QCoreApplication>
#include <QHostAddress>
#include <QStringBuilder>
#include <signon-plugins/sasldata.h>
#include "trgettext.h"

SmtpTask::SmtpTask(QObject *parent, QObject *account):
    MailServiceTask(parent, account),
    m_capabilities(SmtpCapability::None),
    m_started(false),
    m_finished(false),
    m_success(false)
{
    connect(this, &MailServiceTask::stateChanged, this, &SmtpTask::handleNewState);
    connect(this, &MailServiceTask::sessionDataAvailable, this, &SmtpTask::handleSessionResponse);
}

QString SmtpTask::settingsKey(SmtpTask::Setting setting)
{
    switch (setting) {
    case Setting::HOSTNAME:
        return QLatin1String("smtp/server/hostname");
    case Setting::PORT:
        return QLatin1String("smtp/server/port");
    case Setting::USE_SSL:
        return QLatin1String("smtp/encryption/useSSL");
    case Setting::USE_STARTTLS:
        return QLatin1String("smtp/encryption/useStartTLS");
    case Setting::SHOULD_AUTHENTICATE:
        return QLatin1String("smtp/authentication/shouldAuthenticate");
    case Setting::SASL_MECHANISM:
        return QLatin1String("smtp/authentication/sasl/mechanism");
    case Setting::CREDENTIALSID:
        return QLatin1String("smtp/CredentialsId");
    case Setting::ACCEPT_SELFSIGNED_CERT:
        return QLatin1String("smtp/certificates/acceptSelfSigned");
    case Setting::ACCEPT_UNTRUSTED_CERT:
        return QLatin1String("smtp/certificates/acceptUntrusted");
    case Setting::PUB_KEY:
        return QLatin1String("smtp/certificates/currentPubKey");
        break;
    }
}

void SmtpTask::handleNewState(const MailServiceTask::State &state)
{
    switch(state) {
    case State::INVALID:
        setState(State::INIT);
        if (!m_started) {
            m_started = true;
        }
        break;
    case State::INIT:
        initIdentity((quint32)m_account->valueAsInt(settingsKey(Setting::CREDENTIALSID)));
        setState(State::CONNECT_TO_HOST);
        break;
    case State::CONNECT_TO_HOST:
        connectToHost();
        break;
    case State::AUTHENTICATE:
        sendEhlo(true);
        break;
    case State::BUILD_RESULT:
        break;
    case State::COMPLETE:
    case State::FAILED:
        emit result(m_result);
        break;
    }
}

bool SmtpTask::connectToHost()
{
    qDebug() << "CONNECT TO HOST";
    bool useSSL = m_account->valueAsBool(settingsKey(Setting::USE_SSL));
    bool useStartTLS = m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS));
    initSocket((useSSL || useStartTLS));
    connect(m_socket.data(), &QAbstractSocket::readyRead, this, &SmtpTask::handleSocketReadyRead);
    // Add and empty command to the queue to catch the initial response
    SmtpCommand cmd;
    cmd.type = CommandType::InitialGreeting;
    cmd.status = SmtpCommand::CmdStatus::Running;
    m_queue.enqueue(cmd);
    QString host = m_account->valueAsString(settingsKey(Setting::HOSTNAME));
    int port = m_account->valueAsInt(settingsKey(Setting::PORT));
    if (useSSL) {
        ((QSslSocket *) m_socket.data())->connectToHostEncrypted(host, port);
    } else {
        m_socket->connectToHost(host, port);
    }
    return true;
}

bool SmtpTask::authenticate()
{
    qDebug() << "START AUTHENTICATE";
    SignOn::SessionData sessionData(m_service->authData().parameters());
    SaslPluginNS::SaslData saslData = sessionData.data<SaslPluginNS::SaslData>();
    // params for sasl_client_new
    saslData.setService("smtp");
    saslData.setFqdn(m_account->valueAsString(settingsKey(Setting::HOSTNAME)));
    saslData.setRealm(saslData.Fqdn());

    // params for sasl_client_start this is where we let the plugin
    // know about supported mechanisms by server
    QString mech = m_account->valueAsString(settingsKey(Setting::SASL_MECHANISM));
    QString authMechs;
    // Check if we should let the sasl plugin decide on the best mechanism.
    if (mech == QLatin1String("AUTOMATIC") || mech.isEmpty()) {
        // Sasl expects the mechanisms as a space seperated string
        m_authenticationMethods.removeDuplicates();
        authMechs = m_authenticationMethods.join(" ");
    } else {
        if (m_authenticationMethods.contains(mech, Qt::CaseInsensitive)) {
            authMechs = mech;
        } else {
            buildResult(false, "smtp", trGettext("Your SMTP server doesn't support authentication method: %1").arg(mech));
            setState(State::FAILED);
            return false;
        }
    }
#ifdef TASK_DEBUG
    qDebug() << "SMTP MECHS: " << authMechs;
#endif
    m_session->process(saslData, authMechs);
}

void SmtpTask::handleSessionResponse()
{
    SaslPluginNS::SaslData data = m_sessionData.data<SaslPluginNS::SaslData>();
#ifdef TASK_DEBUG
    qDebug() << "SMTP >> SASL DATA: " << data.toMap();
#endif
    SmtpCommand cmd;
    cmd.type = CommandType::Authenticate;
    if (!data.Response().isEmpty()) {
        // Send the response we have been given
        cmd.command = data.Response().toBase64();
    } else {
        // sasl plugin returns empty response for the initial auth step
        // So send it now
        cmd.command = "AUTH " + data.ChosenMechanism().toUtf8();
    }
    queueCommand(cmd);
}

void SmtpTask::handleSocketReadyRead()
{
    if (!m_socket->canReadLine()) {
        return;
    }
    SmtpCommand &cmd = m_queue.head();
    while (m_socket->canReadLine()) {
        QByteArray line = m_socket->readLine();
#ifdef TASK_DEBUG
        qDebug() << "SMTP << " << line;
#endif
        int code = line.left(3).toInt();
        // All response lines should have the same StatusCode
        if (cmd.response.code == StatusCode::Invalid) {
            qDebug() << "Settings status code: " << code;
            cmd.response.code = (StatusCode)code;
        }
        if ((StatusCode)code != cmd.response.code) {
            error(SmtpError(SmtpError::MismatchStatusCode));
            return;
        }
        QByteArray msgLine;
        if (line[3] == '-') {
            msgLine = line.right(line.length() - 4);
            cmd.response.responses.append(msgLine);
        }

        if (line[3] == ' ') { // end of response
            if (cmd.response.isAuthChallenge()) {
                cmd.response.finalResponse = line;
            } else {
                cmd.response.finalResponse = line.right(line.length() - 4);
            }
            break;
        }
    }

    m_queue.head().status = SmtpCommand::CmdStatus::Finished;
    auto updatePubKey = [=](){
          if (m_account->valueAsBool(settingsKey(Setting::USE_SSL)) || m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS))) {
              if (((QSslSocket*) m_socket.data())->isEncrypted()) {
                  m_account->setValue(settingsKey(Setting::PUB_KEY), QLatin1String(((QSslSocket*) m_socket.data())->peerCertificateChain().at(0).toPem()));
              }
          }
    };
    switch (cmd.type) {
    case CommandType::InitialGreeting:
            if (cmd.response.code != StatusCode::ServiceReady) {
                error(SmtpError(cmd.response, SmtpError::UnexpectedResponse));
                return ;
            }
            // Update PUB_KEY now
            updatePubKey();
            setState(State::AUTHENTICATE);
        break;
    case CommandType::Ehlo:
    case CommandType::Helo:
        if (cmd.response.code != StatusCode::Ok) {
            // If EHLO failed send HELO
            if (cmd.type == CommandType::Ehlo) {
                sendEhlo(false);
                return;
            } else {
                error(SmtpError(cmd.response, SmtpError::UnexpectedStatusCode));
                return;
            }
        } else {
            if (m_startedTLS) {
                updatePubKey();
            }
            m_capabilities = SmtpCapability::None;
            m_authenticationMethods.clear();
            QStringList respLines = cmd.response.responses.split("\r\n");
            // Don't forget the final response
            QString final = cmd.response.finalResponse.remove("\r\n");
            respLines.append(final);
#ifdef TASK_DEBUG
            qDebug() << "PARSED LINES: " << respLines;
#endif
            for (int i = 0; i < respLines.length(); ++i) {
                QString cap = respLines.at(i).trimmed().toUpper();
#ifdef TASK_DEBUG
                qDebug() << "CAPABILTIY" << cap;
#endif
                QString auth1 = "AUTH=";
                QString auth2 = "AUTH";

                auto parseMechanisms = [&](const QString &capabilities, const QString &command) {
                    m_capabilities |= SmtpCapability::Authentication;
                    QString mechs = capabilities.right(capabilities.length() - command.length());
                    Q_FOREACH(const QString &mech, mechs.split(" ", QString::SkipEmptyParts)) {
                        m_authenticationMethods.append(mech);
                    }
                };

                if (cap.startsWith(auth1)) {
                    parseMechanisms(cap, auth1);
                } else if (cap.startsWith(auth2)) {
                    parseMechanisms(cap, auth2);
                } else if (cap.startsWith("SIZE")) {
                    m_capabilities |= SmtpCapability::Size;
                } else if (cap == "DSN") {
                    m_capabilities |= SmtpCapability::Dsn;
                } else if (cap == "BINARYMIME") {
                    m_capabilities |= SmtpCapability::BinaryMime;
                } else if (cap == "CHUNKING") {
                    m_capabilities |= SmtpCapability::Chunking;
                } else if (cap == "ENHANCEDSTATUSCODES") {
                    m_capabilities |= SmtpCapability::EnhancedStatusCodes;
                } else if (cap == "8BITMIME") {
                    m_capabilities |= SmtpCapability::EightBitMime;
                } else if (cap == "PIPELINING") {
                    m_capabilities |= SmtpCapability::Pipelining;
                } else if (cap == "STARTTLS") {
                    m_capabilities |= SmtpCapability::StartTLS;
                } else if (cap == "SMTPUTF8") {
                    m_capabilities |= SmtpCapability::UTF8;
                }
            }
        }
        if (m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS))) {
            // silently ignore this account setting if the server caps don't
            // list STARTTLS. We are mos likely going to fail on authenticate
            // next, which will give us the *correct* error response so
            // just wait for that.
            if ((m_capabilities & SmtpCapability::StartTLS) && !m_startedTLS) {
                sendStartTLS();
                return;
            }
        }
        authenticate();
        break;
    case CommandType::StartTLS:
        if (cmd.response.code != StatusCode::ServiceReady) {
            error(SmtpError(cmd.response, SmtpError::UnexpectedResponse));
            return;
        }
        m_startedTLS = true;

        connect(((QSslSocket*) m_socket.data()), &QSslSocket::encrypted, [this](){ sendEhlo(true);} );
        ((QSslSocket*) m_socket.data())->startClientEncryption();
        break;
    case CommandType::Authenticate:
        if (cmd.response.isAuthChallenge()) {
            SaslPluginNS::SaslData data = m_sessionData.data<SaslPluginNS::SaslData>();
            data.setChallenge(cmd.response.finalResponse.toUtf8());
            m_session->process(data, data.ChosenMechanism());
        } else if (cmd.response.code == StatusCode::AuthenticationSuccessful) {
            // were authenticated
            buildResult(true, "smtp");
            setState(State::COMPLETE);
            return;
        } else {
            error(SmtpError(cmd.response));
            return;
        }
        break;
    case CommandType::Invalid:
        break;
    }
}

void SmtpTask::sendEhlo(bool ehlo)
{
    SmtpCommand cmd;
    cmd.type = ehlo ? CommandType::Ehlo : CommandType::Helo;
    cmd.command = ehlo ? "EHLO " : "HELO ";
    QString ip = m_socket->localAddress().toString();
    if (ip.isEmpty()) {
        ip = "localhost";
    }
    QString domain = QString("[" % ip % "]");
    cmd.command.append(domain);
    return queueCommand(cmd);
}

void SmtpTask::sendStartTLS()
{
    SmtpCommand cmd;
    cmd.type = CommandType::StartTLS;
    cmd.command = "STARTTLS";
    queueCommand(cmd);
}

void SmtpTask::queueCommand(SmtpCommand cmd)
{
    m_queue.enqueue(cmd);
    if (m_queue.head().status == SmtpCommand::CmdStatus::Finished) {
        processNextCommand();
    }
}

void SmtpTask::processNextCommand()
{
    if (!m_queue.isEmpty() && m_queue.head().status == SmtpCommand::CmdStatus::Finished) {
        m_queue.dequeue();
    }
    if (m_queue.isEmpty()) {
        return;
    }
    m_queue.head().status = SmtpCommand::CmdStatus::Running;
    writeToSocket(m_queue.head());
}

void SmtpTask::writeToSocket(SmtpCommand cmd)
{
    if (!cmd.command.endsWith("\r\n")) {
        cmd.command.append("\r\n");
    }
#ifdef TASK_DEBUG
    qDebug() << "SMTP >> " << cmd.command;
#endif
    m_socket->write(cmd.command);
}

void SmtpTask::error(SmtpError error)
{
    auto responseCodeToString = [](const StatusCode code){
        int c = (int)code;
        return QString::number(c);
    };

    if (error.type != SmtpError::Invalid) {
        if (error.type == SmtpError::UnexpectedStatusCode) {
            buildResult(false, "smtp", error.response.finalResponse, responseCodeToString(error.response.code));
        } else {
            buildResult(false, "smtp");
        }
    } else {
        buildResult(false, "smtp", error.response.finalResponse, responseCodeToString(error.response.code));
    }
    setState(State::FAILED);
}


void SmtpTask::allowCertificate(const QVariantMap &map)
{
    if (map.value("allow").toBool()) {
        if (((QSslSocket *)m_socket.data())->sslErrors().contains(QSslError::SelfSignedCertificate)) {
            m_account->setValue(settingsKey(Setting::ACCEPT_SELFSIGNED_CERT), true);
        } else if (((QSslSocket *)m_socket.data())->sslErrors().contains(QSslError::CertificateUntrusted)) {
            m_account->setValue(settingsKey(Setting::ACCEPT_UNTRUSTED_CERT), true);
        }
        ((QSslSocket *)m_socket.data())->close();
        setState(State::CONNECT_TO_HOST);
        return;
    }
    buildResult(false, "smtp", ((QSslSocket *)m_socket.data())->errorString());
    setState(State::FAILED);
    return;
}

void SmtpTask::handleSslErrors(const QList<QSslError> &errors)
{
    const bool acceptSelfSigned = m_account->valueAsBool(settingsKey(Setting::ACCEPT_SELFSIGNED_CERT));
    const bool acceptUntrusted = m_account->valueAsBool(settingsKey(Setting::ACCEPT_UNTRUSTED_CERT));
    QList<QSslCertificate> certs = ((QSslSocket *)m_socket.data())->peerCertificateChain();
    QByteArray pubKey = m_account->value(settingsKey(Setting::PUB_KEY)).toByteArray();
    if (errors.contains(QSslError::SelfSignedCertificate)) {
        if (acceptSelfSigned) {
            ((QSslSocket *)m_socket.data())->ignoreSslErrors();
        } else {
            emit verifyCertificate(buildCertMap(errors, certs, pubKey, "imap"));
        }
        return;
    } else if (errors.contains(QSslError::CertificateUntrusted)) {
        if (acceptUntrusted) {
            ((QSslSocket *)m_socket.data())->ignoreSslErrors();
            return;
        } else {
            emit verifyCertificate(buildCertMap(errors, certs, pubKey, "imap"));
        }
    } else {
        buildResult(false, "[smtp]socket", ((QSslSocket *)m_socket.data())->errorString());
        setState(State::FAILED);
        return;
    }
}
