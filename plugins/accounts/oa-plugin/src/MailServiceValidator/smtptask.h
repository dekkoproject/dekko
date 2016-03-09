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
#ifndef SMTPTASK_H
#define SMTPTASK_H

#include <QObject>
#include <QtNetwork/QSslSocket>
#include <QSslError>
#include <QSslCertificate>
#include <QPointer>
#include <QQueue>
#include "mailservicetask.h"

enum class StatusCode {
    Invalid = 0,
    SystemStatus = 211,
    HelpMessage = 214,
    ServiceReady = 220,
    ServiceClosingTransmissionChannel = 221,
    AuthenticationSuccessful = 235,
    Ok = 250,
    UserNotLocalWillForward = 251,
    CannotVerifyUserWillAttemptDelivery = 252,
    AuthenticationChallenge = 334,
    StartMailInput = 354,
    ServiceNotAvailable = 421,
    PasswordTransitionNeeded = 432,
    MailboxBusy = 450,
    ErrorInProcessing = 451,
    InsufficientStorage = 452,
    TemporaryAuthenticationFailure = 454,
    CommandUnrecognized = 500,
    SyntaxError = 501,
    CommandNotImplemented = 502,
    BadCommandSequence = 503,
    CommandParameterNotImplemented = 504,
    AuthenticationRequired = 530,
    AuthenticationMechanismTooWeak = 534,
    EncryptionRequiredForAuthenticationMechanism = 538,
    MailboxUnavailable = 550,
    UserNotLocalTryAlternatePath = 551,
    ExceededStorageAllocation = 552,
    MailboxNameNotAllowed = 553,
    TransactionFailed = 554,
};

enum class CommandType {
    Invalid,
    InitialGreeting,
    StartTLS,
    Ehlo,
    Helo,
    Authenticate
};

struct SmtpResponse {
    StatusCode code = StatusCode::Invalid;
    QString finalResponse; // the final response i.e "250 XYZ"
    // The actual responses we received, we may need to enumerate over these when interacting with the sasl plugin
    // these are all mid responses i.e "250-XYZ" (notice the '-' )
    QString responses;

    bool isAuthChallenge() {
        return code == StatusCode::AuthenticationChallenge;
    }
};

struct SmtpCommand {
    enum CmdStatus { Created, Running, Finished, Error };
    CommandType type = CommandType::Invalid;
    CmdStatus status = CmdStatus::Created;
    QByteArray command;
    SmtpResponse response;

    bool isRunning() {
        return status == Running;
    }
};

struct SmtpError {
    enum Type {
        Invalid,
        UnexpectedResponse,
        UnexpectedStatusCode,
        SendDataTimeOut,
        ResponseTimeOut,
        MismatchStatusCode,
        ConnectionTimeOut,
        SaslPluginError
    };
    Type type;
    SmtpResponse response;
    SmtpError (): type(Invalid), response(SmtpResponse()) {}
    SmtpError (Type _type): type(_type), response(SmtpResponse()) {}
    SmtpError (SmtpResponse _resp): response(_resp), type(Invalid) {}
    SmtpError (SmtpResponse _resp, Type _type): response(_resp), type(_type) {}
};

class SmtpTask : public MailServiceTask
{
    Q_OBJECT
public:
    SmtpTask(QObject *parent, QObject *account);

    enum SmtpCapability {
        None = 0,
        Size = 1 << 0,
        Dsn = 1 << 1,
        EnhancedStatusCodes = 1 << 2,
        Authentication = 1 << 3,
        EightBitMime = 1 << 4,
        Pipelining = 1 << 5,
        BinaryMime = 1 << 6,
        Chunking = 1 << 7,
        StartTLS = 1 << 8,
        UTF8 = 1 << 9
    };
    Q_DECLARE_FLAGS(SmtpCapabilities, SmtpCapability)


    // These are already defined as slots in MailServiceTask
    // so we don't need to define them as slots again and save MOC some work
    // by just defining them as public
    virtual void allowCertificate(const QVariantMap &map);
    virtual void handleSslErrors(const QList<QSslError> &errors);

public slots:
    void handleNewState(const MailServiceTask::State &state);
    void handleSessionResponse();
    void handleSocketReadyRead();

protected:
    QString settingsKey(Setting setting);
    virtual bool connectToHost();
    virtual bool authenticate();

private:
    void sendEhlo(bool ehlo);
    void sendStartTLS();
    void queueCommand(SmtpCommand cmd);
    void processNextCommand();
    void writeToSocket(SmtpCommand cmd);
    void error(SmtpError error);

    // Members
    SmtpCapabilities m_capabilities;
    QStringList m_authenticationMethods;
    QQueue<SmtpCommand> m_queue;
    bool m_started;
    bool m_finished;
    bool m_success;
    bool m_authSent;
    bool m_startedTLS;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(SmtpTask::SmtpCapabilities)

#endif // SMTPTASK_H
