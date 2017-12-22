/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

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
#include <QDBusConnection>
#include "ClientService.h"
#include <qmailstore.h>
#include "Folder.h"
#include "MailServiceInterface.h"


class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *service READ service NOTIFY serviceChanged)
    Q_PROPERTY(bool hasConfiguredAccounts READ hasConfiguredAccounts CONSTANT)
    Q_PROPERTY(bool hasUndoableActions READ hasUndoableActions NOTIFY undoCountChanged)
    Q_PROPERTY(QString undoableActionDescription READ undoDescription NOTIFY undoCountChanged)
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

    bool hasUndoableActions() const;
    QString undoDescription() const;

    QObject *service() const { return m_service; }
    bool hasConfiguredAccounts(); // account service

    Q_INVOKABLE void deleteMessage(const int &msgId); // done
    Q_INVOKABLE void restoreMessage(const int &msgId); // done
    void deleteMessages(const QMailMessageIdList &idList); // done

    Q_INVOKABLE void synchronizeAccount(const int &id); // account service
    void synchronizeAccount(const QMailAccountId &id); // account service

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

    Q_INVOKABLE void createStandardFolders(const quint64 &accountId);
    void createStandardFolders(const QMailAccountId &accountId);
    Q_INVOKABLE void moveToStandardFolder(const int &msgId, const int &standardFolder); // standardFolder is a Folder::FolderType
    void moveToStandardFolder(const QMailMessageIdList &msgIds, const Folder::FolderType &folder, const bool userTriggered = true);
    Q_INVOKABLE void moveToFolder(const quint64 &msgId, const quint64 &folderId);
    void moveToFolder(const QMailMessageIdList &ids, const QMailFolderId &folderId);

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
    bool removeMessage(const QMailMessageId &id, const QMailStore::MessageRemovalOption &option);
//    void addMessages(const QMailMessageList &msgList);
//    void updateMessages(const QMailMessageList &msgList);

    // C++ Submission api
    void sendMessage(const QMailMessage &msg);
    Q_INVOKABLE void sendPendingMessages();

    Q_INVOKABLE void undoActions();

    MailServiceInterface *bus() { return m_mService; }

    void pruneCache(const QMailMessageIdList &msgIds);

signals:
    void undoCountChanged();
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
    void standardFoldersCreated(const quint64 &accountId, const bool &created);

public slots:
    void handleFailure(const quint64 &id, const int &statusCode, const QString &statusText);

private slots:
    void handleMessagesNowAvailable(const QList<quint64> &msgIds);
    void handleMessageFetchFailed(const QList<quint64> &msgIds);
    void handleMessagesSent(const QList<quint64> &msgIds);
    void handleMessageSendingFailed(const QList<quint64> &msgIds, const int &error);

protected:
    QMailAccountIdList getEnabledAccountIds() const;

private:

    ClientService *m_service;
    MailServiceInterface *m_mService;

};

#endif // CLIENT_H
