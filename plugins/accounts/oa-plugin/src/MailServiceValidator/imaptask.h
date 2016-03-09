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
#ifndef IMAPTASK_H
#define IMAPTASK_H

#include <QObject>
#include <QPointer>
#include <QSslSocket>
#include <QQueue>
#include "mailservicetask.h"

enum class CommandResponse {
    None,
    Ok,
    Bad,
    No,
    Capability,
    NotSupportedResponse // Use this for all responses we arent interested in
};

enum class Command {
    INVALID,
    InitialGreeting,
    StartTLS,
    Capability,
    Authenticate
};

struct ImapResponse {
    QString tag;
    CommandResponse type;
    QStringList args; // Extracted response arguments after tag & command have been removed
    QByteArray buffer; // Raw response buffer, used to pass the raw value to sasl plugin
    bool isUnTagged() {
        return tag == QLatin1String("*");
    }
    bool isContinuation() {
        return tag == QLatin1String("+");
    }
};

struct ImapCommand {
    enum CmdStatus { Created, Queued, Running, Finished, Error };
    Command type = Command::INVALID;
    CmdStatus status;
    QString tag;
    QByteArray command;
    ImapResponse response;
};

struct ImapError {
    enum Type {
        Invalid,
        UnexpectedResponse,
        UnexpectedStatusCode,
        SendDataTimeOut,
        ResponseTimeOut,
        ConnectionTimeOut,
        SaslPluginError
    };
    Type type;
    ImapCommand command;
    ImapError () : type(Invalid), command(ImapCommand()){}
    ImapError (Type _type) : type(_type), command(ImapCommand()){}
    ImapError (ImapCommand _resp) : command(_resp), type(Invalid){}
    ImapError (ImapCommand _resp, Type _type): command(_resp), type(_type){}
};

class ImapTask : public MailServiceTask
{
    Q_OBJECT
public:
    ImapTask(QObject *parent, QObject *&account);

    enum ImapCapability {
        None = 0,
        IMAP4 = 1 << 0,
        IMAP4rev1 = 1 << 1,
        StartTLS = 1 << 2,
        SaslIR = 1 << 3,
        LoginDisabled = 1 << 4
    };
    Q_DECLARE_FLAGS(ImapCapabilities, ImapCapability)

    // These are already defined as slots in MailServiceTask
    // so we don't need to define them as slots again and save MOC some work
    // by just defining them as public
    virtual void allowCertificate(const QVariantMap &map);
    virtual void handleSslErrors(const QList<QSslError> &errors);


public slots:
    void handleNewState(const MailServiceTask::State &state);
    void initialSessionResponse();
    void handleSocketReadyRead();

protected:
    virtual QString settingsKey(Setting setting);
    virtual bool connectToHost();
    virtual bool authenticate();

    ImapCommand buildResponse(const QByteArray &resp);
    void parseCapabilties(const ImapCommand &cmd);
    void requestCapabilities(); // In case we didnt get them as part of the initial greeting

private:
    QString nextTag();
    void error(ImapError error);
    void processNextCommand();
    void queueCommand(ImapCommand command);
    void writeToSocket(ImapCommand command);

    QQueue<ImapCommand> m_queue;
    ImapCapabilities m_capabilities;
    QStringList m_authenticationMethods;
    QString m_tag;
    int m_tagNum;
    bool m_waitingForGreeting;
    bool m_requestedCaps;
    bool m_startedTLS;
    bool m_initialResponseSent;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ImapTask::ImapCapabilities)
#endif // IMAPTASK_H
