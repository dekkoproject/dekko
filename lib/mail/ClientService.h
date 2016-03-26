#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QObject>
#include <QTimer>
#include <QPointer>
#include <QmlObjectListModel.h>
#include <PriorityQueue.h>
#include <qmailserviceaction.h>
#include "ClientServiceAction.h"

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

signals:
    void undoCountChanged();
    void updatesRolledBack();
    void queueChanged();

public slots:
    void undoActions();


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

public slots:
    void activityChanged(QMailServiceAction::Activity activity){
        if (QMailServiceAction *action = m_queue->first()->action()) {
            if (activity == QMailServiceAction::Successful) {
                if (action->metaObject()->className() == QStringLiteral("QMailRetrievalAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::ExportAction) {
                        m_queue->dequeue();
                        qDebug() << "Export action complete";
                    }
                    emit processNext();
                }
            } else if (activity == QMailServiceAction::Failed) {
                const QMailServiceAction::Status status(action->status());
                if (action->metaObject()->className() == QStringLiteral("QMailRetrievalAction")) {
                    if (m_queue->first()->serviceActionType() == ClientServiceAction::ExportAction) {
                        qDebug() << "Export action failed " << status.accountId << "Reason: " << status.text;
                        // because this is a silent action. i.e the changes can be exported in a future
                        // export it's not critical to interrupt the UI right now. Let's just dequeue the job
                        m_queue->dequeue();
                        emit processNext();
                    }
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
