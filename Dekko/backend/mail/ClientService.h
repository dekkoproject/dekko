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
#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QObject>
#include <QTimer>
#include <QPointer>
#include <QmlObjectListModel.h>
#include <PriorityQueue.h>
#include <qmailserviceaction.h>
#include <qmailaccount.h>
#include "ClientServiceAction.h"
#include "Folder.h"

class ClientServiceWatcher;
/** @short ClientService is responsible for all message related actions
 *
 * Created actions are placed in priority based queues and their lifetime is managed by this component.
 *
 * There shouldn't be any need to directly use this class but rather the exposed methods of Client singleton.
 *
 * The queues are available as listmodels to provide easy qml debug widgets when trying to diagnose queue based issues
 * since the listmodels are qlist based and pretty fast there's no real impact to using them
 */
class ClientService : public QObject
{
    Q_OBJECT
    /** @short list model of recent actions that are undoable */
    Q_PROPERTY(QObject *undoActionQueue READ undoQueue NOTIFY queueChanged)
    Q_PROPERTY(bool hasUndoableAction READ hasUndoableAction NOTIFY undoCountChanged)
    Q_PROPERTY(QString undoableActionDescription READ undoDescription NOTIFY undoCountChanged)

    /** @short model of queued service actions */
    Q_PROPERTY(QObject *serviceActionQueue READ serviceQueue NOTIFY queueChanged)

public:
    explicit ClientService(QObject *parent = 0);
    QObject *undoQueue() { return m_undoQueue; }
    QObject *serviceQueue() { return m_serviceQueue; }
    bool hasUndoableAction();
    QString undoDescription();
    void deleteMessages(const QMailMessageIdList &ids);
    void restoreMessage(const QMailMessageId &id);
    void markMessagesImportant(const QMailMessageIdList &msgIds, const bool important);
    void markMessagesRead(const QMailMessageIdList &msgIds, const bool read);
    void markMessagesTodo(const QMailMessageIdList &msgIds, const bool todo);
    void markMessagesDone(const QMailMessageIdList &msgIds, const bool done);
    void markMessagesReplied(const QMailMessageIdList &idList, const bool all);
    void markMessageForwarded(const QMailMessageIdList &idList);
    void markFolderRead(const QMailFolderId &id);
    void downloadMessagePart(const QMailMessageId &id, const QString &location);
    void downloadMessages(const QMailMessageIdList &msgIds);
    void sendMessage(const QMailMessage &msg);
    void createStandardFolders(const QMailAccountId &id);
    void moveToStandardFolder(const QMailMessageIdList &msgIds, const Folder::FolderType &folder, const bool userTriggered);
    void moveToFolder(const QMailMessageIdList &msgIds, const QMailFolderId &folder);
    void synchronizeAccount(const QMailAccountId &id);
    void emptyTrash(const QMailAccountIdList &ids);
    void syncFolders(const QMailAccountId &accountId, const QMailFolderIdList &folders);

signals:
    void undoCountChanged();
    void updatesRolledBack();
    void queueChanged();
    void messagePartFetched(const quint64 &message, const QString &location);
    void messagePartFetchFailed(const quint64 &message, const QString &location);
    void messagesFetched(const QMailMessageIdList &ids);
    void messageFetchFailed(const QMailMessageIdList &ids);
    void messagesSent(const QMailMessageIdList &ids);
    void messageSendingFailed(const QMailMessageIdList &ids, QMailServiceAction::Status::ErrorCode error);
    void accountSynced(const quint64 &id);
    void syncAccountFailed(const quint64 &id);
    void actionFailed(const quint64 &id, const QMailServiceAction::Status &status);
    void standardFoldersCreated(const quint64 &id, const bool created);

public slots:
    void undoActions();
    void sendAnyQueuedMail();

private slots:
    void processNextAction();
    /** @short Process the next action in the action queue */
    void processNextServiceAction();
    /** @short Get's called on ClientServiceAction::complete() */
    void exportMailStoreUpdate();
    void exportMailStoreUpdate(const QMailAccountIdList &ids);
    void exportMailStoreUpdate(const QMailAccountId &id);

    void undoableCountChanged();
    void rollBackMailStoreUpdates(const QMailAccountIdList &accounts);

private:
    void connectServiceAction(QMailServiceAction* action);
    void enqueue(ClientServiceAction *action);
    bool exportQueuedForAccountId(const QMailAccountId &id);

private:
    QQmlObjectListModel<ClientServiceAction> *m_undoQueue;
    QQmlObjectListModel<ClientServiceAction> *m_serviceQueue;
    QPointer<ClientServiceWatcher> m_serviceWatcher;
    QQmlObjectListModel<ClientServiceAction> *m_storageQueue;
    QQmlObjectListModel<ClientServiceAction> *m_transmitQueue;
    QTimer *m_undoTimer;

    QMailRetrievalAction *m_exportAction;
};

/** @short ClientServiceWatcher watches the activity of a queues running action
 *
 * It will dequeue on a action finishing and call for the next action to be processed
 */
class ClientServiceWatcher : public QObject
{
    Q_OBJECT
public:
    explicit ClientServiceWatcher(QObject *parent = 0) : QObject(parent) {}

    void setWatchQueue(QQmlObjectListModel<ClientServiceAction> *queue) { m_queue = queue; }

signals:
    void processNext();
    void messagePartFetched(const quint64 &message, const QString &location);
    void messagePartFetchFailed(const quint64 &message, const QString &location);
    void messagesFetched(const QMailMessageIdList &ids);
    void messageFetchFailed(const QMailMessageIdList &ids);
    void checkSendMailQueue();
    void messagesSent();
    void messageSendingFailed();
    void accountSynced(const quint64 &id);
    void syncAccountFailed(const quint64 &id);
    void standardFoldersCreated(const quint64 &id, const bool created);
    void actionFailed(const quint64 &id, const QMailServiceAction::Status &status);

public slots:
    void activityChanged(QMailServiceAction::Activity activity){
        if (QMailServiceAction *action = m_queue->first()->action()) {
            if (activity == QMailServiceAction::Successful) {
                qDebug() << "Service action successful";
                if (action->metaObject()->className() == QStringLiteral("QMailRetrievalAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::ExportAction) {
                        //                        qDebug() << "Export action complete";
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::RetrievePartAction) {
                        //                        qDebug() << "FetchPart successful";
                        ClientServiceAction *clientAction = m_queue->at(0);
                        emit messagePartFetched(clientAction->messageId(), clientAction->location());
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::RetrieveAction) {
                        //                        qDebug() << "Fetch messages successful";
                        ClientServiceAction *clientAction = m_queue->at(0);
                        emit messagesFetched(clientAction->messageIds());
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::SyncAccountAction) {
                        //                        qDebug() << "Sync account successful";
                        ClientServiceAction *clientAction = m_queue->at(0);
                        emit accountSynced(clientAction->accountId().toULongLong());
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::CreateStandardFolders) {
                        ClientServiceAction *clientAction = m_queue->at(0);
                        emit standardFoldersCreated(clientAction->accountId().toULongLong(), true);
                    }
                    m_queue->dequeue();
                    emit processNext();
                } else if (action->metaObject()->className() == QStringLiteral("QMailStorageAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::OutboxAction) {
                        qDebug() << "Message stored in outbox";
                        emit checkSendMailQueue();
                    }
                    m_queue->dequeue();
                    emit processNext();

                } else if (action->metaObject()->className() == QStringLiteral("QMailTransmitAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::SendAction) {
                        qDebug() << "Success sending pending messages";
                        emit messagesSent();
                    }
                    m_queue->dequeue();
                    emit processNext();
                }

            } else if (activity == QMailServiceAction::Failed) {
                const QMailServiceAction::Status status(action->status());
                if (action->metaObject()->className() == QStringLiteral("QMailRetrievalAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::ExportAction) {
                        qDebug() << "Export action failed " << status.accountId << "Reason: " << status.text;
                        // because this is a silent action. i.e the changes can be exported in a future
                        // export it's not critical to interrupt the UI right now. Let's just dequeue the job
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::RetrievePartAction) {
                        ClientServiceAction *fetch = m_queue->at(0);
                        emit messagePartFetchFailed(fetch->messageId(), fetch->location());
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::RetrieveAction) {
                        ClientServiceAction *clientAction = m_queue->at(0);
                        emit messageFetchFailed(clientAction->messageIds());
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::SyncAccountAction) {
                        //                        qDebug() << "Sync account failed";
                        ClientServiceAction *clientAction = m_queue->at(0);
                        emit syncAccountFailed(clientAction->accountId().toULongLong());
                        emit actionFailed(clientAction->accountId().toULongLong(), action->status());
                    } else if (m_queue->first()->serviceActionType() == ClientServiceAction::CreateStandardFolders) {
                        ClientServiceAction *clientAction = m_queue->at(0);
                        emit standardFoldersCreated(clientAction->accountId().toULongLong(), false);
                    }
                    m_queue->dequeue();
                    emit processNext();
                } else if (action->metaObject()->className() == QStringLiteral("QMailStorageAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::OutboxAction) {
                        qDebug() << "Failed while storing message in outbox: " <<  action->status().text;
                        emit checkSendMailQueue();
                    }
                    m_queue->dequeue();
                    emit processNext();
                } else if (action->metaObject()->className() == QStringLiteral("QMailTransmitAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::SendAction) {
                        qDebug() << "Failed sending pending messages: " << action->status().text;
                        emit messageSendingFailed();
                    }
                    m_queue->dequeue();
                    emit processNext();
                }
            }
        } else {
            qDebug() << "Not sure what this is here :-(";
        }
    }

private:
    QPointer<QQmlObjectListModel<ClientServiceAction> > m_queue;

};

#endif // CLIENTSERVICE_H
