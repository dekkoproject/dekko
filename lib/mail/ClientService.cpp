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
    m_serviceWatcher = new ClientServiceWatcher(this);
    m_serviceWatcher->setWatchQueue(m_serviceQueue);
    connect(m_serviceWatcher, &ClientServiceWatcher::processNext, this, &ClientService::processNextAction);
    connect(m_serviceWatcher, &ClientServiceWatcher::messagePartFetched, this, &ClientService::messagePartFetched);
    connect(m_serviceWatcher, &ClientServiceWatcher::messagePartFetchFailed, this, &ClientService::messagePartFetchFailed);
    connect(m_serviceWatcher, &ClientServiceWatcher::messagesFetched, this, &ClientService::messagesFetched);
    connect(m_serviceWatcher, &ClientServiceWatcher::messageFetchFailed, this, &ClientService::messageFetchFailed);
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
    qDebug() << "Deleting " << ids.count() << "messages";
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

void ClientService::markMessagesImportant(const QMailMessageIdList &msgIds, const bool important)
{
    if (msgIds.isEmpty()) {
        return;
    }
    FlagsAction *flagAction = new FlagsAction(this, msgIds, FlagsAction::FlagStarred, important ? FlagsAction::Apply : FlagsAction::Remove);
    flagAction->process();
    exportMailStoreUpdate(flagAction->accountIds());
    flagAction->deleteLater();
}

void ClientService::markMessagesRead(const QMailMessageIdList &msgIds, const bool read)
{
    if (msgIds.isEmpty()) {
        return;
    }
    FlagsAction *flagAction = new FlagsAction(this, msgIds, FlagsAction::FlagRead, read ? FlagsAction::Apply : FlagsAction::Remove);
    flagAction->process();
    exportMailStoreUpdate(flagAction->accountIds());
    flagAction->deleteLater();
}

void ClientService::markMessagesTodo(const QMailMessageIdList &msgIds, const bool todo)
{
    if (msgIds.isEmpty()) {
        return;
    }
    FlagsAction *flagAction = new FlagsAction(this, msgIds, FlagsAction::FlagTodo, todo ? FlagsAction::Apply : FlagsAction::Remove);
    flagAction->process();
    exportMailStoreUpdate(flagAction->accountIds());
    flagAction->deleteLater();
}

void ClientService::markMessagesDone(const QMailMessageIdList &msgIds, const bool done)
{
    if (msgIds.isEmpty()) {
        return;
    }
    QMailMessageIdList todosToRemove;
    Q_FOREACH(auto &id, msgIds) {
        QMailMessage msg(id);
        msg.setCustomField("task-done", QString::number(done ? 1 : 0));
        if ((msg.status() & QMailMessage::Todo)) {
            todosToRemove << id;
        }
        QMailStore::instance()->updateMessage(&msg);
    }
    if (!todosToRemove.isEmpty()) {
        markMessagesTodo(todosToRemove, !done);
    }
}

void ClientService::downloadMessagePart(const QMailMessagePart *part)
{
    if (!part->location().isValid(true)) {
        return;
    }
    enqueue(new FetchMessagePartAction(this, part));
}

void ClientService::downloadMessages(const QMailMessageIdList &msgIds)
{
    if (msgIds.isEmpty()) {
        return;
    }
    enqueue(new FetchMessagesAction(this, msgIds));
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


void ClientService::undoActions()
{
    m_undoTimer->stop();
    if (!m_undoQueue->isEmpty()) {
        // get list of accountids to rollback updates on
        // first get the total number of enabled accounts.
        int size = QMailStore::instance()->queryAccounts(QMailAccountKey::messageType(QMailMessage::Email)
                                                  & QMailAccountKey::status(QMailAccount::Enabled),
                                                  QMailAccountSortKey::name()).count();

        QMailAccountIdList accounts;
        foreach(ClientServiceAction *action, m_undoQueue->toList()) {
            Q_FOREACH(const QMailAccountId &id, qobject_cast<UndoableAction *>(action)->accountIds()) {
                if (!accounts.contains(id)) {
                    accounts.append(id);
                    if (accounts.count() == size) {
                        break; // early
                    }
                }
            }
        }
        rollBackMailStoreUpdates(accounts);
        m_undoQueue->clear();
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
    exportMailStoreUpdate(accounts);
    m_undoQueue->clear();
}

void ClientService::exportMailStoreUpdate(const QMailAccountIdList &ids)
{
    foreach(const QMailAccountId &id, ids) {
        exportMailStoreUpdate(id);
    }
}

void ClientService::exportMailStoreUpdate(const QMailAccountId &id)
{
    if (id.isValid()) {
        qDebug() << "Valid account id: " << id.toULongLong();
        if (!exportQueuedForAccountId(id)) {
            enqueue(new ExportUpdatesAction(this, id));
        }
    }
}

void ClientService::enqueue(ClientServiceAction *action)
{
    const bool queueWasEmpty = m_serviceQueue->isEmpty();
    qDebug() << "Enqueuing action";
    m_serviceQueue->enqueue(action);
    if (queueWasEmpty) {
        qDebug() << "Queue was empty processing next action";
        processNextServiceAction();
    }
}

bool ClientService::exportQueuedForAccountId(const QMailAccountId &id)
{
    bool queued = false;
    Q_FOREACH(ClientServiceAction *action, m_serviceQueue->toList()) {
        if (action->metaObject()->className() == QStringLiteral("ExportUpdatesAction")) {
            ExportUpdatesAction *exportAction = static_cast<ExportUpdatesAction *>(action);
            if (exportAction->accountId() == id) {
                if (action == m_serviceQueue->first()) {
                    if (m_serviceQueue->first()->isRunning()) {
                        qDebug() << "Action queued but currently running, so queue another";
                        queued = false; // already running so we need to queue another
                        continue;
                    }
                }
                qDebug() << "Action already queued for "<< id;
                queued = true; // already queued;
            }
        }
    }
    return queued;
}

void ClientService::processNextAction()
{
    QTimer::singleShot(0, this, SLOT(processNextServiceAction()));
}

void ClientService::processNextServiceAction()
{
    if (m_serviceQueue->isEmpty()) {
        qDebug() << "Action queue empty nothing to do :-)";
        return;
    }
    qDebug() << "Processing next service action;";
    qDebug() << "Queue size is: " << m_serviceQueue->size();
    if (!m_serviceQueue->first()->isRunning()) {
        connect(m_serviceQueue->first(), &ClientServiceAction::activityChanged, m_serviceWatcher, &ClientServiceWatcher::activityChanged);
        m_serviceQueue->first()->process();
    } else {
        qDebug() << "Action already running, cannot start another until it's done.";
    }
}
