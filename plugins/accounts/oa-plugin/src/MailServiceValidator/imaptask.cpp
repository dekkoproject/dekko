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
#include "imaptask.h"
#include <QStringBuilder>
#include <QCoreApplication>
#include <QDebug>
#include "trgettext.h"


ImapTask::ImapTask(QObject *parent, QObject* &account) :
    MailServiceTask(parent, account),
    m_tagNum(0),
    m_waitingForGreeting(false),
    m_requestedCaps(false),
    m_startedTLS(false)
{
    connect(this, &MailServiceTask::stateChanged, this, &ImapTask::handleNewState);
    connect(this, &MailServiceTask::sessionDataAvailable, this, &ImapTask::initialSessionResponse);
}

QString ImapTask::settingsKey(Setting setting)
{
    switch (setting) {
    case Setting::HOSTNAME:
        return QLatin1String("imap/server/hostname");
    case Setting::PORT:
        return QLatin1String("imap/server/port");
    case Setting::USE_SSL:
        return QLatin1String("imap/encryption/useSSL");
    case Setting::USE_STARTTLS:
        return QLatin1String("imap/encryption/useStartTLS");
    case Setting::SHOULD_AUTHENTICATE:
        return QLatin1String("imap/authentication/shouldAuthenticate");
    case Setting::SASL_MECHANISM:
        return QLatin1String("imap/authentication/sasl/mechanism");
    case Setting::ACCEPT_SELFSIGNED_CERT:
        return QLatin1String("imap/certificates/acceptSelfSigned");
    case Setting::ACCEPT_UNTRUSTED_CERT:
        return QLatin1String("imap/certificates/acceptUntrusted");
    case Setting::PUB_KEY:
        return QLatin1String("imap/certificates/currentPubKey");
        break;
    }
}

void ImapTask::handleNewState(const MailServiceTask::State &state)
{
    switch(state) {
    case State::INVALID:
        setState(State::INIT);
        break;
    case State::INIT:
        // Do we need to do any intitialising or resets here??
        initIdentity(m_account->credentialsId());
        setState(State::CONNECT_TO_HOST);
        break;
    case State::CONNECT_TO_HOST:
        connectToHost();
        break;
    case State::AUTHENTICATE:
        authenticate();
        break;
    case State::BUILD_RESULT:
        break;
    case State::COMPLETE:
    case State::FAILED:
        emit result(m_result);
        break;
    }
}

bool ImapTask::connectToHost()
{
    m_capabilities = ImapCapability::None;
    m_authenticationMethods.clear();
    bool useSSL = m_account->valueAsBool(settingsKey(Setting::USE_SSL));
    bool useStartTLS = m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS));
    initSocket((useSSL || useStartTLS));
    connect(m_socket.data(), &QAbstractSocket::readyRead, this, &ImapTask::handleSocketReadyRead);
    // Add an empty command to the queue as we just want to catch the initial response;
    // THis won't get executed in processNextCommand
    ImapCommand cmd;
    cmd.type = Command::InitialGreeting;
    cmd.tag = "*";
    cmd.status = ImapCommand::CmdStatus::Running;
    queueCommand(cmd);
    QString host = m_account->valueAsString(settingsKey(Setting::HOSTNAME));
    int port = m_account->valueAsInt(settingsKey(Setting::PORT));
    if (useSSL) {
        ((QSslSocket *) m_socket.data())->connectToHostEncrypted(host, port);
    } else {
        m_socket->connectToHost(host, port);
    }
    return true;
}

bool ImapTask::authenticate()
{
    if (m_capabilities == ImapCapability::None) {
        // The server didn't send them on the initial greeting so request them now
        requestCapabilities();
        return false;
    }
    // First we have to check for LOGINDISABLED on an unencrypted connection
    // We need to correct the error here and switch to the correct socket type.
    if (!m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS)) &&
            !m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS))) {
        // TODO: should we actually return error about LOGINDISABLED and needing to issue STARTTLS
        // For convenience to the user let's correct the mistake, but we should probably tell them about it??
        if ((m_capabilities & ImapCapability::LoginDisabled)) {
            // Update the account
            m_account->setValue(settingsKey(Setting::USE_STARTTLS), true);
            m_account->sync();
            // We need to switch to a QSslSocket now
            m_socket->close();
            m_socket.clear();
            // Kick a new process
            setState(State::CONNECT_TO_HOST);
            return false;
        }
    }
    // Initiate starttls if required
    if (m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS)) || (m_capabilities & ImapCapability::LoginDisabled)) {
        // silently ignore this account setting if the server caps don't
        // list STARTTLS. We are most likely going to fail on authenticate
        // next, which will give us the *correct* error response so
        // just wait for that.
        if ((m_capabilities & ImapCapability::StartTLS && !m_startedTLS)) {
            ImapCommand cmd;
            cmd.type = Command::StartTLS;
            cmd.command = "STARTTLS";
            queueCommand(cmd);
            return false;
        }
    }
    // start sasl process;
    SignOn::SessionData sessionData(m_service->authData().parameters());
    SaslPluginNS::SaslData saslData = sessionData.data<SaslPluginNS::SaslData>();
    // params for sasl_client_new
    saslData.setService("imap");
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
        if (m_authenticationMethods.contains(mech)) {
            authMechs = mech;
        } else {
            buildResult(false, "imap", trGettext("Your IMAP server doesn't support authentication method: %1").arg(mech));
            setState(State::FAILED);
            return false;
        }
    }
    m_session->process(saslData, authMechs);
    return true;
}

void ImapTask::initialSessionResponse()
{
    SaslPluginNS::SaslData data = m_sessionData.data<SaslPluginNS::SaslData>();
    if (!data.Response().isEmpty()) {
        // Send the response we have been given
        ImapCommand ic;
        ic.type = Command::Authenticate;
        ic.command = "AUTHENTICATE " + data.ChosenMechanism().toUtf8();
        bool initialResponseSent = false;
        if ((m_capabilities & ImapCapability::SaslIR)) {
            ic.command += " " + data.Response().toBase64();
            initialResponseSent = true;
        }
        queueCommand(ic);
    } else {
        error(ImapError(ImapError::SaslPluginError));
        return;
    }
}

void ImapTask::handleSocketReadyRead()
{
    ImapCommand cmd;
    while (m_socket->canReadLine()) {
        cmd = buildResponse(m_socket->readLine());
#ifdef TASK_DEBUG
        qDebug() << "IMAP << " << cmd.response.buffer;
#endif
        // First check to see if this is untagged, as we may be waiting
        // for the initial greeting or this is our chance to grab the capabilties
        if (cmd.response.isUnTagged()) {
            if (cmd.type == Command::InitialGreeting && cmd.response.type == CommandResponse::Ok) {
                break;
            }
            if (cmd.response.type == CommandResponse::Capability) {
                parseCapabilties(cmd);
            }
        }
        // If we aren't waiting on the initial greeting then we are waiting
        // on the command we sent with m_tag or if this is a continuation we need to return
        // to finish sending the initial response
        if (cmd.response.tag == m_tag.trimmed() || cmd.response.isContinuation()) {
            break;
        }
    }
    // Finish this command before queuing any more
    m_queue.head().status = ImapCommand::CmdStatus::Finished;
    // ensure the current pub key is up to date. This provides a level of guarantee that
    // this accounts settings are correct for the certificate with *this* public key.
    // Clients using this account should compare public keys and ensure that it gets updated
    // if the certificates change or get rotated (usually due to a security issue on the server)
    // also the user is to be prompted about a certificate change if the certificate is untrusted/self-signed
    // and the current settings prevent auto acceptance.
    auto updatePubKey = [=](){
          if (m_account->valueAsBool(settingsKey(Setting::USE_SSL)) || m_account->valueAsBool(settingsKey(Setting::USE_STARTTLS))) {
              if (((QSslSocket*) m_socket.data())->isEncrypted()) {
                  m_account->setValue(settingsKey(Setting::PUB_KEY), QLatin1String(((QSslSocket*) m_socket.data())->peerCertificateChain().at(0).toPem()));
              }
          }
    };
    switch(cmd.type) {
    case Command::InitialGreeting:
        if (cmd.response.type == CommandResponse::Ok) {
// Uncomment to test verifying SSL if you don't have a service with a self signed certificate view
#if 0
            emit verifyCertificate(buildCertMap(((QSslSocket*) m_socket.data())->sslErrors(), ((QSslSocket*) m_socket.data())->peerCertificateChain(), QByteArray(), "imap"));
            return;
#endif
            // Update PUB_KEY now
            updatePubKey();
            setState(State::AUTHENTICATE);
        }
        break;
    case Command::Capability:
    case Command::StartTLS:
        if (cmd.response.type == CommandResponse::Ok) {
            if (cmd.type == Command::StartTLS) {
                m_startedTLS = true;
                connect(((QSslSocket*) m_socket.data()), &QSslSocket::encrypted, [this](){ requestCapabilities();} );
                ((QSslSocket*) m_socket.data())->startClientEncryption();
                return;
            }
            updatePubKey();
            authenticate();
        } else {
            error(ImapError(cmd));
        }
        break;
    case Command::Authenticate:
        if (cmd.response.isContinuation()) {
            SaslPluginNS::SaslData data = m_sessionData.data<SaslPluginNS::SaslData>();
            if (!m_initialResponseSent) {
                QByteArray ir = data.Response().toBase64();
                m_initialResponseSent = true;
                ImapCommand ic;
                ic.type = Command::Authenticate;
                ic.command = ir;
                ic.tag = "+";
                queueCommand(ic);
                return;
            } else {
                data.setChallenge(QByteArray::fromBase64(cmd.response.buffer));
                m_session->process(data, data.ChosenMechanism());
                return;
            }
        }
        if (cmd.response.type == CommandResponse::Ok) {
            // were authenticated
            buildResult(true, "imap");
            setState(State::COMPLETE);
        } else {
            error(ImapError(cmd));
        }
        break;
    }
}

ImapCommand ImapTask::buildResponse(const QByteArray &resp)
{
    ImapCommand &cmd = m_queue.head();
    // first check if it's a continuation response.
    if (resp.startsWith("+")) {
        cmd.response.tag = "+";
        cmd.response.buffer = resp;
        return cmd;
    }
    QStringList respList = QString::fromLatin1(resp).split(" ");
    // First we determine the tag this will either be a * or UOA{int}
    cmd.response.tag = respList.takeFirst();
    // Next figure out the command response status
    QString r = respList.takeFirst();
    if (r == "OK") {
        cmd.response.type = CommandResponse::Ok;
    } else if (r == "BAD") {
        cmd.response.type = CommandResponse::Bad;
    } else if (r == "NO") {
        cmd.response.type = CommandResponse::No;
    } else if (r == "CAPABILITY" || r == "[CAPABILITY]") {
        cmd.response.type = CommandResponse::Capability;
    } else {
        // This could be anything such as an EXISTS or what not
        // Just set the response to not supported as it's not intereseting
        // to this particular task :-)
        cmd.response.type = CommandResponse::NotSupportedResponse;
    }
    cmd.response.args = respList;
    cmd.response.buffer = resp;
    return cmd;
}

void ImapTask::parseCapabilties(const ImapCommand &cmd)
{
    if (cmd.response.args.isEmpty()) {
        //TODO: emit error
    }
    Q_FOREACH(const QString &cap, cmd.response.args) {
        if (cap.startsWith("AUTH=")) {
            QString auth = cap;
            auth = auth.remove("\r\n");
            m_authenticationMethods.append(auth.remove("AUTH="));
        } else if (cap == "IMAP4") {
            m_capabilities |= ImapCapability::IMAP4;
        } else if (cap == "IMAP4rev1") {
            m_capabilities |= ImapCapability::IMAP4rev1;
        } else if (cap == "STARTTLS") {
            m_capabilities |= ImapCapability::StartTLS;
        } else if (cap == "SASL-IR") {
            m_capabilities |= ImapCapability::SaslIR;
        } else if (cap == "LOGINDISABLED") {
            m_capabilities |= ImapCapability::LoginDisabled;
        }
    }
}

void ImapTask::requestCapabilities()
{
    if (m_requestedCaps) {
        return;
    }
    m_requestedCaps = true;
    m_capabilities = ImapCapability::None;
    m_authenticationMethods.clear();
    ImapCommand ic;
    ic.type = Command::Capability;
    ic.command = "CAPABILITY";
    queueCommand(ic);
}

QString ImapTask::nextTag()
{
    ++m_tagNum;
    m_tag = QString("UOA%1 ").arg(m_tagNum);
    return m_tag;
}

void ImapTask::error(ImapError error)
{

    if (error.type != ImapError::Invalid) {
        if (error.type == ImapError::UnexpectedStatusCode) {
            buildResult(false, "imap", error.command.response.buffer, error.command.tag);
        } else {
            buildResult(false, "imap");
        }
    } else {
        buildResult(false, "imap", error.command.response.buffer, error.command.tag);
    }
    setState(State::FAILED);
}

void ImapTask::processNextCommand()
{
    if (!m_queue.isEmpty() && m_queue.head().status == ImapCommand::CmdStatus::Finished) {
        m_queue.dequeue();
    }
    if (m_queue.isEmpty()) {
        return;
    }
    m_queue.head().status = ImapCommand::CmdStatus::Running;
    writeToSocket(m_queue.head());
}

void ImapTask::queueCommand(ImapCommand command)
{
    m_queue.enqueue(command);
    if (m_queue.head().status == ImapCommand::CmdStatus::Finished)
        processNextCommand();
}

void ImapTask::writeToSocket(ImapCommand command)
{
    if (!command.command.endsWith("\r\n")) {
        QByteArray newLine = command.command + "\r\n";
        command.command = newLine;
    }
    if (command.tag != "+") {
        QString tag = nextTag();
        command.command.prepend(tag.toUtf8());
        command.tag = tag;
    }
#ifdef TASK_DEBUG
    qDebug() << "IMAP >> " << command.command;
#endif
    m_socket->write(command.command);
    command.status = ImapCommand::CmdStatus::Running;
}

void ImapTask::allowCertificate(const QVariantMap &map)
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
    buildResult(false, "imap", ((QSslSocket *)m_socket.data())->errorString());
    setState(State::FAILED);
    return;
}

void ImapTask::handleSslErrors(const QList<QSslError> &errors)
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
        buildResult(false, "[imap]socket", ((QSslSocket *)m_socket.data())->errorString());
        setState(State::FAILED);
        return;
    }
}
