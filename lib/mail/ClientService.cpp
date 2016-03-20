#include "ClientService.h"
#include <qmailstore.h>
#include <qmaildisconnected.h>

ClientService::ClientService(QObject *parent) : QObject(parent),
    m_undoQueue(0), m_undoTimer(0), m_exportAction(0)
{
    m_undoTimer = new QTimer(this);
    m_undoTimer->setInterval(5000);
    m_undoTimer->setSingleShot(true);
    connect(m_undoTimer, SIGNAL(timeout()), this, SLOT(exportMailStoreUpdate()));
    m_undoQueue = new QQmlObjectListModel<ClientServiceAction>(this);
    connect(m_undoQueue, &QQmlObjectListModel<ClientServiceAction>::countChanged,
            this, &ClientService::undoableCountChanged);
    m_serviceQueue = new QQmlObjectListModel<ClientServiceAction>(this);
    emit queueChanged();
}

bool ClientService::hasUndoableAction()
{
    bool hasUndoAction = false;
    foreach(ClientServiceAction *action, m_undoQueue->toList()) {
        if (action->actionType() == ClientServiceAction::Undoable) {
            hasUndoAction = true;
        }
    }
    return hasUndoAction;
}

QString ClientService::undoDescription()
{
    int undoCount = 0;
    bool isDeleteAction = false;
    foreach(ClientServiceAction *action, m_undoQueue->toList()) {
        if (action->actionType() == ClientServiceAction::Undoable) {
            undoCount += qobject_cast<UndoableAction *>(action)->itemCount();
            if (action->serviceActionType() == ClientServiceAction::DeleteAction) {
                isDeleteAction = true;
            }
        }
    }

    if (isDeleteAction) {
        QString msg = undoCount > 1 ? tr("messages") : tr("message");
        return QStringLiteral("%1 %2 deleted").arg(QString::number(undoCount), msg);
    }
    return QString();
}

void ClientService::deleteMessages(const QMailMessageIdList &ids)
{
    if (ids.isEmpty()) {
        return;
    }
    // these are undoable so add to the undo queue once processed
    DeleteMessagesAction *action = new DeleteMessagesAction(this, ids);
    action->process();
    m_undoQueue->append(action);
}

void ClientService::restoreMessage(const QMailMessageId &id)
{
    if (id.isValid()) {
        QMailDisconnected::restoreToPreviousFolder(id);
        exportMailStoreUpdate(QMailMessage(id).parentAccountId());
    }
}

void ClientService::exportMailStoreUpdate()
{
    QMailAccountIdList accounts;
    foreach(ClientServiceAction *action, m_undoQueue->toList()) {
        Q_FOREACH(const QMailAccountId &id, qobject_cast<UndoableAction *>(action)->accountIds()) {
            if (!accounts.contains(id)) {
                accounts.append(id);
            }
        }
    }
    foreach(const QMailAccountId &id, accounts) {
        exportMailStoreUpdate(id);
    }
    m_undoQueue->clear();
}
void ClientService::exportMailStoreUpdate(const QMailAccountId &id)
{
    if (id.isValid()) {
        if (!m_exportAction) {
            m_exportAction = new QMailRetrievalAction(this);
            connectServiceAction(m_exportAction);
        }
        m_serviceQueue->append(new ExportUpdatesAction(0, m_exportAction, id));

        if (!m_exportAction->isRunning()) {
            processNextServiceAction();
        }
    }
}

void ClientService::undoableCountChanged()
{
    emit undoCountChanged();
    if (m_undoQueue->isEmpty()) {
        m_undoTimer->stop();
        return; // 19772
    } else if (m_undoTimer->isActive()) {
        // restart the timer
        m_undoTimer->stop();
    }
    m_undoTimer->start();
}

void ClientService::rollBackMailStoreUpdates(const QMailAccountIdList &accounts)
{
    if (accounts.isEmpty()) {
        return;
    }
    Q_FOREACH(auto &id, accounts) {
        if (QMailDisconnected::updatesOutstanding(id)) {
            QMailDisconnected::rollBackUpdates(id);
        }
        emit updatesRolledBack();
    }
}

void ClientService::activityChanged(QMailServiceAction::Activity activity)
{
    if (QMailServiceAction *action = qobject_cast<QMailServiceAction*>(sender())) {
        if (activity == QMailServiceAction::Successful) {
            /*if (action == m_transmitAction) {
                transmitCompleted();
            } else if (action == m_retrievalAction) {
                retrievalCompleted();
            } else if (action->metaObject()->className() == QString("QMailStorageAction")) {
                storageActionCompleted();
                action->deleteLater();
            } else */
            if (action == m_exportAction) {
                qDebug() << "Export action successful";
                m_serviceQueue->remove(0); // finished successfully
//                clearStatusText();
                processNextServiceAction();
            }
        } else if (activity == QMailServiceAction::Failed) {
//            const QMailServiceAction::Status status(action->status());
            if (action == m_exportAction) {
                qDebug() << "Export action failed";
            }
//            if (action->metaObject()->className() == QString("QMailStorageAction")) {
//                storageActionFailure(status.accountId, status.text);
//                action->deleteLater();
//            } else if (action == m_exportAction) {
//                rollBackUpdates(status.accountId);
//            } else {
//                transferFailure(status.accountId, status.text, status.errorCode);
//            }
        }
    }
}

void ClientService::connectServiceAction(QMailServiceAction *action)
{
//    connect(action, SIGNAL(connectivityChanged(QMailServiceAction::Connectivity)), this, SLOT(connectivityChanged(QMailServiceAction::Connectivity)));
    connect(action, SIGNAL(activityChanged(QMailServiceAction::Activity)), this, SLOT(activityChanged(QMailServiceAction::Activity)));
//    connect(action, SIGNAL(statusChanged(QMailServiceAction::Status)), this, SLOT(statusChanged(QMailServiceAction::Status)));
//    connect(action, SIGNAL(progressChanged(uint, uint)), this, SLOT(progressChanged(uint, uint)));
}

void ClientService::undoActions()
{
    m_undoTimer->stop();
    if (!m_undoQueue->isEmpty()) {
        // get list of accountids to rollback updates on
        QMailAccountIdList accounts;
        foreach(ClientServiceAction *action, m_undoQueue->toList()) {
            Q_FOREACH(const QMailAccountId &id, qobject_cast<UndoableAction *>(action)->accountIds()) {
                if (!accounts.contains(id)) {
                    accounts.append(id);
                }
            }
        }
        rollBackMailStoreUpdates(accounts);
        m_undoQueue->clear();
    }
}

void ClientService::processNextServiceAction()
{
    if (m_serviceQueue->isEmpty()) {
        m_exportAction->deleteLater();
        m_exportAction = 0;
        return;
    }

    if (!m_exportAction->isRunning()) {
        m_serviceQueue->at(0)->process();
    }
}

