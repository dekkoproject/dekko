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
#include "mailservicetask.h"
#include "trgettext.h"


MailServiceTask::MailServiceTask(QObject *parent, QObject *account) :
    QObject(parent),m_accountId(0), m_state(INVALID)
{
    m_account = qobject_cast<Accounts::Account*>(account);
    m_service = new Accounts::AccountService(m_account.data(), Accounts::Service(), this);
}

MailServiceTask::~MailServiceTask()
{
    // sync anything we've changed during the auth process
    m_account->sync();
}

MailServiceTask::State MailServiceTask::state() const { return m_state; }

void MailServiceTask::setState(const MailServiceTask::State &state) {
    if (state != m_state) {
        m_state = state;
        emit stateChanged(m_state);
    }
}

void MailServiceTask::run() {
    setState(State::INIT);
}

void MailServiceTask::initIdentity(const quint32 &credentialsId) {
    m_identity = SignOn::Identity::existingIdentity(credentialsId, this);
    if (m_identity.data()) {
        m_session = m_identity->createSession("sasl");
        if (m_session.isNull()) {
            qFatal("[MailServiceTask::initIdentity] Invalid session");
        }
        connect(m_session, &SignOn::AuthSession::response, [=](const SignOn::SessionData &data) {
#ifdef TASK_DEBUG
            qDebug() << "SESSION DATA AVAILABLE: " << data.toMap();
#endif
            m_sessionData = data;
            emit sessionDataAvailable();
        });
        connect(m_session, &SignOn::AuthSession::error, [this](const SignOn::Error &error){
            qDebug() << "SIGNON ERROR: " << error.message();
            buildResult(false, "signon", error.message(), QString::number(error.type()));
            setState(State::FAILED);
        });
    }
}

void MailServiceTask::initSocket(const bool useSSL) {
    if (useSSL) {
        m_socket = new QSslSocket(this);
        connect(((QSslSocket*) m_socket.data()), SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(handleSslErrors(QList<QSslError>)));
    } else {
        m_socket = new QTcpSocket(this);
    }
}

void MailServiceTask::buildResult(const bool pass, const QString &protocol, const QString &reason, const QString &errorCode) {
    m_result["success"] = pass;
    m_result["reason"] = reason;
    m_result["code"] = errorCode;
    m_result["protocol"] = protocol;
}

QVariantMap MailServiceTask::buildCertMap(const QList<QSslError> &errors, const QList<QSslCertificate> &certs,
                                const QByteArray &pubKey, const QString &protocol) {
    QVariantMap map;
    map["allow"] = false; // Should be set to true when returned to allow this certificate
    map["protocol"] = protocol;
    map["pub_key"] = certs.at(0).publicKey().toPem();
    map["self_signed"] = errors.contains(QSslError::SelfSignedCertificate);
    map["untrusted"] = errors.contains(QSslError::CertificateUntrusted);
    QString message;
    bool pubKeyHasChanged = !pubKey.isEmpty() && (certs.isEmpty() || certs.at(0).publicKey().toPem() != pubKey);
    map["certificate_changed"] = pubKeyHasChanged;
    if (pubKeyHasChanged) {
        // We have a new untrusted certificate, this could possibly mean
        // that the change in certificate isn't genuine and we need to prompt about it.
        message = trGettext("<p>The public key of the SSL certificate of the %1 server has changed since the last time "
                     "and your system doesn't believe that the new certificate is genuine.</p>\n%2\n%3\n"
                     "<p>Would you like to connect anyway and remember the new certificate?</p>").
               arg(protocol.toUpper(), sslChainToHtml(certs), sslErrorsToHtml(errors));

    } else {
        // First time we've seen these certificates
        message = trGettext("<p>This is the first time you're connecting to this %1 server and the server certificate failed "
                                  "validation test.</p>\n%2\n\n%3\n"
                                  "<p>Would you like to connect and remember this certificate's public key for the next time?</p>")
                            .arg(protocol.toUpper(), sslChainToHtml(certs), sslErrorsToHtml(errors));

    }
    map["certificate_display"] = message;
    return map;
}



QByteArray MailServiceTask::htmlHexifyByteArray(const QByteArray &rawInput)
{
    QByteArray inHex = rawInput.toHex();
    QByteArray res;
    const int stepping = 4;
    for (int i = 0; i < inHex.length(); i += stepping) {
        res.append("<code style=\"font-family: monospace;\">");
        res.append(inHex.mid(i, stepping));
        if (i + stepping < inHex.size()) {
            res.append(":");
        }
        res.append("</code><span style=\"font-size: 1px\"> </span>");
    }
    return res;
}

QString MailServiceTask::sslChainToHtml(const QList<QSslCertificate> &sslChain)
{
    QStringList certificateStrings;
    Q_FOREACH(const QSslCertificate &cert, sslChain) {
        certificateStrings << QObject::tr("<li><b>CN</b>: %1,<br/>\n<b>Organization</b>: %2,<br/>\n"
                                 "<b>Serial</b>: %3,<br/>\n"
                                 "<b>SHA1</b>: %4,<br/>\n<b>MD5</b>: %5</li>").arg(
                                  cert.subjectInfo(QSslCertificate::CommonName).join(", ").toHtmlEscaped(),
                                  cert.subjectInfo(QSslCertificate::Organization).join(", ").toHtmlEscaped(),
                                  cert.serialNumber(),
                                  htmlHexifyByteArray(cert.digest(QCryptographicHash::Sha1)),
                                  htmlHexifyByteArray(cert.digest(QCryptographicHash::Md5)));
    }
    return sslChain.isEmpty() ?
                QObject::tr("<p>The remote side doesn't have a certificate.</p>\n") :
                QObject::tr("<p>This is the certificate chain of the connection:</p>\n<ul>%1</ul>\n").arg(certificateStrings.join("\n"));
}

QString MailServiceTask::sslErrorsToHtml(const QList<QSslError> &sslErrors)
{
    QStringList sslErrorStrings;
    Q_FOREACH(const QSslError &e, sslErrors) {
        sslErrorStrings << QString("<li>%1</li>").arg(e.errorString().toHtmlEscaped());
    }
    return sslErrors.isEmpty() ?
                QObject::tr("<p>According to your system's policy, this connection is secure.</p>\n") :
                QObject::tr("<p>The connection triggered the following SSL errors:</p>\n<ul>%1</ul>\n").arg(sslErrorStrings.join("\n"));
}

