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
#ifndef MAILSERVICETASK_H
#define MAILSERVICETASK_H

#include <QObject>
#include <QDebug>
#include <QPointer>
#include <QSslSocket>
#include <QSslKey>
#include <Accounts/Account>
#include <Accounts/AccountService>
#include <Accounts/AuthData>
#include <SignOn/AuthSession>
#include <SignOn/Identity>
#include <SignOn/SessionData>
#include <signon-plugins/sasldata.h>

// Enables debug output for the socket IO
// WARNING: shouldn't be enabled for release builds.
// will possibly contain sensitive information depending on the
// sasl mechanism used.
#define TASK_DEBUG

using namespace SignOn;

class MailServiceTask : public QObject
{
    Q_OBJECT
public:
    MailServiceTask(QObject *parent, QObject *account);
    ~MailServiceTask();

    enum State {
        INVALID,
        INIT,
        CONNECT_TO_HOST,
        AUTHENTICATE,
        BUILD_RESULT,
        COMPLETE,
        FAILED
    };

    enum Setting {
        HOSTNAME,
        PORT,
        USE_SSL,
        USE_STARTTLS,
        SHOULD_AUTHENTICATE,
        SASL_MECHANISM,
        CREDENTIALSID,
        ACCEPT_SELFSIGNED_CERT,
        ACCEPT_UNTRUSTED_CERT,
        PUB_KEY
    };

signals:
    // Emitted when we hit either a selfsigned or untrusted certificate
    // The emitted cert map should be returned via allowCertificates with the "allow" value set to true.
    void verifyCertificate(const QVariantMap &cert);
    void error(QString error);
    void stateChanged(const MailServiceTask::State state);
    void sessionDataAvailable();
    void result(const QVariantMap &map);

public slots:
    void run();
    virtual void allowCertificate(const QVariantMap &map) = 0;
    virtual void handleSslErrors(const QList<QSslError> &errors) = 0;

protected:
    virtual QString settingsKey(Setting setting) = 0;
    virtual bool connectToHost() = 0;
    virtual bool authenticate() = 0;

    State state() const;
    void setState(const State &state);

    // Initializes the SignOn identity for session data
    void initIdentity(const quint32 &credentialsId);
    // Initializes a new QTcp/QSsl Socket
    void initSocket(const bool useSSL);

    void buildResult(const bool pass, const QString &protocol, const QString &reason = QString(), const QString &errorCode = QString());

    QVariantMap buildCertMap(const QList<QSslError> &errors, const QList<QSslCertificate> &certs,
                      const QByteArray &pubKey, const QString &protocol);

    const quint32 m_accountId;
    QPointer<QTcpSocket> m_socket;
    QPointer<Accounts::Account> m_account;
    QPointer<Accounts::AccountService> m_service;
    QPointer<SignOn::AuthSession> m_session;
    QPointer<SignOn::Identity> m_identity;
    SignOn::SessionData m_sessionData;
    State m_state;
    QVariantMap m_result;

private:
    QString sslErrorsToHtml(const QList<QSslError> &sslErrors);
    QString sslChainToHtml(const QList<QSslCertificate> &sslChain);
    QByteArray htmlHexifyByteArray(const QByteArray &rawInput);
};

Q_DECLARE_METATYPE(MailServiceTask::State)
#endif // MAILSERVICETASK_H
