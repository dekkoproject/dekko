/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

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
#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include "ClientService.h"
#include <qmailstore.h>
#include "Folder.h"

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *service READ service NOTIFY serviceChanged)
    Q_PROPERTY(bool hasConfiguredAccounts READ hasConfiguredAccounts CONSTANT)
    Q_ENUMS(Error)

public:
    static Client *instance();
    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);
    explicit Client(QObject *parent = 0);

    enum Error {
        NoError,
        FrameworkFault,
        SystemError,
        InternalServerError,
        UnexpectedResponse,
        LoginFailed,
        CancelError,
        FileSystemFull,
        MessageNotExist,
        EnqueueFailed,
        NoConnection,
        ConnectionInUse,
        ConnectionNotReady,
        ConfigurationError,
        InvalidAddress,
        InvalidData,
        TimeoutError,
        InternalStateReset,
        SslNotSupported,
        UntrustedCertificate,
        UnknownError
    };

    QObject *service() const { return m_service; }
    bool hasConfiguredAccounts();

    Q_INVOKABLE void deleteMessage(const int &msgId);
    Q_INVOKABLE void restoreMessage(const int &msgId);
    void deleteMessages(const QMailMessageIdList &idList);

    Q_INVOKABLE void synchronizeAccount(const int &id);
    void synchronizeAccount(const QMailAccountId &id);

    Q_INVOKABLE void syncStandardFolder(const int &standardFolder); //Folder::FolderType
    Q_INVOKABLE void syncStandardFolder(const quint64 &accountId, const int &standardFolder);
    Q_INVOKABLE void syncFolder(const quint64 &accountId, const quint64 &folderId);
    void syncFolders(const QMailAccountId &accountId, const QMailFolderIdList &folders);

    Q_INVOKABLE void markMessageImportant(const int &msgId, const bool important);
    void markMessagesImportant(const QMailMessageIdList &idList, const bool important);

    Q_INVOKABLE void markMessageRead(const int &msgId, const bool read);
    void markMessagesRead(const QMailMessageIdList &idList, const bool read);

    Q_INVOKABLE void markMessageTodo(const int &msgId, const bool todo);
    void markMessagesTodo(const QMailMessageIdList &idList, const bool todo);

    Q_INVOKABLE void markMessageDone(const int &msgId, const bool done);
    void markMessagesDone(const QMailMessageIdList &idList, const bool done);

    Q_INVOKABLE void moveToStandardFolder(const int &msgId, const int &standardFolder); // standardFolder is a Folder::FolderType
    void moveToStandardFolder(const QMailMessageIdList &msgIds, const Folder::FolderType &folder);

    Q_INVOKABLE void emptyTrash();
    Q_INVOKABLE void emptyTrash(const int &accountId);
    void emptyTrash(const QMailAccountIdList &ids);

    Q_INVOKABLE void markStandardFolderRead(const int &standardFolder); // Folder::FolderType
    Q_INVOKABLE void markStandardFolderRead(const quint64 &accountId, const int &standardFolder);
    void markFolderRead(const QMailFolderId &id);

    void markMessagesReplied(const QMailMessageIdList &idList, const bool all);
    void markMessageForwarded(const QMailMessageIdList &idList);

    void downloadMessagePart(const QMailMessagePart *msgPart);
    void downloadMessage(const QMailMessageId &msgId);
    void downloadMessages(const QMailMessageIdList &idList);

    bool addMessage(QMailMessage *msg);
    void removeMessage(const QMailMessageId &id, const QMailStore::MessageRemovalOption &option);
//    void addMessages(const QMailMessageList &msgList);
//    void updateMessages(const QMailMessageList &msgList);

    // C++ Submission api
    void sendMessage(const QMailMessage &msg);

signals:
    void serviceChanged();
    void messagePartNowAvailable(const quint64 &msgId, const QString &partLocation);
    void messagePartFetchFailed(const quint64 &msgId, const QString &partLocation);
    void messagesNowAvailable(const QMailMessageIdList &idList);
    void messageFetchFailed(const QMailMessageIdList &ids);
    void messagesSent(const QMailMessageIdList &ids);
    void messageSendingFailed(const QMailMessageIdList &ids, QMailServiceAction::Status::ErrorCode error);
    void accountSynced(const quint64 &id);
    void syncAccountFailed(const quint64 &id);
    void clientError(const quint64 &accountId, const Error &error, const QString &errorString);

public slots:
    void handleFailure(const quint64 &id, const QMailServiceAction::Status &status);

protected:
    QMailAccountIdList getEnabledAccountIds() const;

private:
    ClientService *m_service;
};

#endif // CLIENT_H
