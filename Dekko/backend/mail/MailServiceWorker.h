/* Copyright (C) 2017 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef MAILSERVICEWORKER_H
#define MAILSERVICEWORKER_H


#include <QObject>
#include <QtDBus>
#include "ClientService.h"
#include <qmailmessagekey.h>
#include <qmailmessagesortkey.h>


class MailServiceWorker : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.dekkoproject.MailService")

public:
    explicit MailServiceWorker(QObject *parent = Q_NULLPTR);

    static void registerTypes();

    Q_PROPERTY(bool hasUndoableAction READ hasUndoableAction)
    bool hasUndoableAction();
    Q_PROPERTY(QString undoDescription READ undoDescription)
    QString undoDescription();

public slots:
    /**
     * @brief deleteMessages
     * @param ids
     */
    void deleteMessages(const QList<quint64> &ids);
    /**
     * @brief restoreMessage
     * @param id
     */
    void restoreMessage(const quint64 &id);
    /**
     * @brief markMessagesImportant
     * @param msgIds
     * @param important
     */
    void markMessagesImportant(const QList<quint64> &msgIds, const bool important);
    /**
     * @brief markMessagesRead
     * @param msgIds
     * @param read
     */
    void markMessagesRead(const QList<quint64> &msgIds, const bool read);
    /**
     * @brief markMessagesTodo
     * @param msgIds
     * @param todo
     */
    void markMessagesTodo(const QList<quint64> &msgIds, const bool todo);
    /**
     * @brief markMessagesDone
     * @param msgIds
     * @param done
     */
    void markMessagesDone(const QList<quint64> &msgIds, const bool done);
    /**
     * @brief markMessagesReplied
     * @param msgIds
     * @param all
     */
    void markMessagesReplied(const QList<quint64> &msgIds, const bool all);
    /**
     * @brief markMessageForwarded
     * @param msgIds
     */
    void markMessageForwarded(const QList<quint64> &msgIds);
    /**
     * @brief syncFolders
     * @param accountId
     * @param folders
     */
    void syncFolders(const quint64 &accountId, const QList<quint64> &folders);
    /**
     * @brief createStandardFolders
     * @param accountId
     */
    void createStandardFolders(const quint64 &accountId);
    /**
     * @brief moveToFolder
     * @param msgIds
     * @param folderId
     */
    void moveToFolder(const QList<quint64> &msgIds, const quint64 &folderId);
    /**
     * @brief moveToStandardFolder
     * @param msgIds
     * @param folder
     * @param userTriggered
     */
    void moveToStandardFolder(const QList<quint64> &msgIds, const int &folder, const bool userTriggered = true);
    /**
     * @brief markFolderRead
     * @param folderId
     */
    void markFolderRead(const quint64 &folderId);
    /**
     * @brief downloadMessagePart
     * @param msgId
     * @param partLocation
     */
    void downloadMessagePart(const quint64 &msgId, const QString &partLocation);
    /**
     * @brief downloadMessages
     * @param msgIds
     */
    void downloadMessages(const QList<quint64> &msgIds);
    /**
     * @brief sendMessage
     * @param msgId
     */
    void sendMessage(const quint64 &msgId);
    /**
     * @brief sendPendingMessages
     */
    void sendPendingMessages();
    /**
     * @brief synchronizeAccount
     * @param accountId
     */
    void synchronizeAccount(const quint64 &accountId);
    /**
     * @brief undoActions
     */
    void undoActions();
    /**
     * @brief sendAnyQueuedMail
     */
    void sendAnyQueuedMail();

    void emptyTrash(const QList<quint64> &accountIds);

    void removeMessage(const quint64 &msgId, const int &option);

    int totalCount(const QByteArray &msgKey);
    QList<quint64> queryMessages(const QByteArray &msgKey, const QByteArray &sortKey, const int &limit);
signals:
    void undoCountChanged();
    void updatesRolledBack();
    void queueChanged();
    void messageRestored(const quint64 &msgId);
    void messagePartNowAvailable(const quint64 &msgId, const QString &partLocation);
    void messagePartFetchFailed(const quint64 &msgId, const QString &partLocation);
    void messagesNowAvailable(const QList<quint64> &idList);
    void messageFetchFailed(const QList<quint64> &ids);
    void messagesSent(const QList<quint64> &ids);
    void messageSendingFailed(const QList<quint64> &ids, const int &error);
    void accountSynced(const quint64 &id);
    void syncAccountFailed(const quint64 &id);
    void clientError(const quint64 &accountId, const int &error, const QString &errorString);
    void standardFoldersCreated(const quint64 &accountId, const bool &created);
    void actionFailed(const quint64 &id, const int &statusCode, const QString &statusText);


private slots:
    void handleMessagesFetched(const QMailMessageIdList &msgIds);
    void handleMessageFetchFailed(const QMailMessageIdList &msgIds);
    void handleMessagesSent(const QMailMessageIdList &msgIds);
    void handleMessageSendingFailed(const QMailMessageIdList &ids, QMailServiceAction::Status::ErrorCode error);
    void handleActionFailed(const quint64 &id, const QMailServiceAction::Status &status);

private:
    QMailMessageKey toMessageKey(const QByteArray &key);
    QMailMessageSortKey toMessageSortKey(const QByteArray &key);
    QMailMessageIdList toMsgIdList(const QList<quint64> &ids);
    QMailFolderIdList toFolderIdList(const QList<quint64> &ids);
    QMailAccountIdList toAccountIdList(const QList<quint64> &ids);
    QList<quint64> toDBusMsgIdList(const QMailMessageIdList &msgIds);

    ClientService *m_service;

};


#endif // MAILSERVICEWORKER_H
