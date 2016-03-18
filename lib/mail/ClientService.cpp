#include "ClientService.h"
#include <qmailstore.h>
#include <qmaildisconnected.h>

ClientService::ClientService(QObject *parent) : QObject(parent),
    m_undoQueue(0), m_undoTimer(0)
{
    m_undoTimer = new QTimer(this);
    m_undoTimer->setInterval(5000);
    m_undoTimer->setSingleShot(true);
    connect(m_undoTimer, &QTimer::timeout, this, &ClientService::exportMailStoreUpdate);
    m_undoQueue = new QQmlObjectListModel<ClientServiceAction>(this);
    connect(m_undoQueue, &QQmlObjectListModel<ClientServiceAction>::countChanged,
            this, &ClientService::undoableCountChanged);
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

void ClientService::exportMailStoreUpdate()
{
}

void ClientService::undoableCountChanged()
{
    emit undoCountChanged();
    if (m_undoQueue->isEmpty()) {
        m_undoTimer->stop();
        return;
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

