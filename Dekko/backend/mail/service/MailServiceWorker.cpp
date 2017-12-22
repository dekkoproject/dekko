#include "MailServiceWorker.h"
#include <QDataStream>
#include <QByteArray>
#include <qmailstore.h>
#include "serviceutils.h"


MailServiceWorker::MailServiceWorker(QObject *parent) : QObject(parent),
    m_service(Q_NULLPTR)
{
    m_service = new ClientService(this);

    connect(m_service, &ClientService::undoCountChanged, this, &MailServiceWorker::undoCountChanged);
    connect(m_service, &ClientService::updatesRolledBack, this, &MailServiceWorker::updatesRolledBack);
    connect(m_service, &ClientService::messagePartFetched, this, &MailServiceWorker::messagePartNowAvailable);
    connect(m_service, &ClientService::messagePartFetchFailed, this, &MailServiceWorker::messagePartFetchFailed);
    connect(m_service, &ClientService::messagesFetched, this, &MailServiceWorker::handleMessagesFetched);
    connect(m_service, &ClientService::messageFetchFailed, this, &MailServiceWorker::handleMessageFetchFailed);
    connect(m_service, &ClientService::messagesSent, this, &MailServiceWorker::handleMessagesSent);
    connect(m_service, &ClientService::messageSendingFailed, this, &MailServiceWorker::handleMessageSendingFailed);
    connect(m_service, &ClientService::accountSynced, this, &MailServiceWorker::accountSynced);
    connect(m_service, &ClientService::syncAccountFailed, this, &MailServiceWorker::syncAccountFailed);
    connect(m_service, &ClientService::actionFailed, this, &MailServiceWorker::handleActionFailed);
    connect(m_service, &ClientService::standardFoldersCreated, this, &MailServiceWorker::standardFoldersCreated);
}

void MailServiceWorker::registerTypes() {
    qRegisterMetaType<QList<quint64>>("QList<quint64>");
    qDBusRegisterMetaType<QList<quint64>>();
}

bool MailServiceWorker::hasUndoableAction()
{
    return m_service->hasUndoableAction();
}

QString MailServiceWorker::undoDescription()
{
    return m_service->undoDescription();
}

void MailServiceWorker::deleteMessages(const QList<quint64> &ids)
{
    QMailMessageIdList mIds = from_dbus_msglist(ids);
    m_service->deleteMessages(mIds);
}

void MailServiceWorker::restoreMessage(const quint64 &id)
{
    QMailMessageId mid(id);
    m_service->restoreMessage(mid);
}

void MailServiceWorker::markMessagesImportant(const QList<quint64> &msgIds, const bool important)
{
    QMailMessageIdList mIds = from_dbus_msglist(msgIds);
    m_service->markMessagesImportant(mIds, important);
}

void MailServiceWorker::markMessagesRead(const QList<quint64> &msgIds, const bool read)
{
    qDebug() << "Marking Message Read: " << msgIds;
    QMailMessageIdList mIds = from_dbus_msglist(msgIds);
    m_service->markMessagesRead(mIds, read);
}

void MailServiceWorker::markMessagesTodo(const QList<quint64> &msgIds, const bool todo)
{
    QMailMessageIdList mIds = from_dbus_msglist(msgIds);
    m_service->markMessagesTodo(mIds, todo);
}

void MailServiceWorker::markMessagesDone(const QList<quint64> &msgIds, const bool done)
{
    QMailMessageIdList mIds = from_dbus_msglist(msgIds);
    m_service->markMessagesDone(mIds, done);
}

void MailServiceWorker::markMessagesReplied(const QList<quint64> &msgIds, const bool all)
{
    QMailMessageIdList list = from_dbus_msglist(msgIds);
    m_service->markMessagesReplied(list, all);
}

void MailServiceWorker::markMessageForwarded(const QList<quint64> &msgIds)
{
    QMailMessageIdList list = from_dbus_msglist(msgIds);
    m_service->markMessageForwarded(list);
}

void MailServiceWorker::syncFolders(const quint64 &accountId, const QList<quint64> &folders)
{
    QMailAccountId id(accountId);
    QMailFolderIdList list = from_dbus_folderlist(folders);
    m_service->syncFolders(id, list);
}

void MailServiceWorker::createStandardFolders(const quint64 &accountId)
{
    QMailAccountId id(accountId);
    m_service->createStandardFolders(id);
}

void MailServiceWorker::moveToFolder(const QList<quint64> &msgIds, const quint64 &folderId)
{
    QMailMessageIdList list = from_dbus_msglist(msgIds);
    QMailFolderId id(folderId);
    m_service->moveToFolder(list, id);
}

void MailServiceWorker::moveToStandardFolder(const QList<quint64> &msgIds, const int &folder, const bool userTriggered)
{
    QMailMessageIdList list = from_dbus_msglist(msgIds);
    Folder::FolderType type = static_cast<Folder::FolderType>(folder);
    m_service->moveToStandardFolder(list, type, userTriggered);
}

void MailServiceWorker::markFolderRead(const quint64 &folderId)
{
    QMailFolderId id(folderId);
    m_service->markFolderRead(id);
}

void MailServiceWorker::downloadMessagePart(const quint64 &msgId, const QString &partLocation)
{
    QMailMessageId id(msgId);
    m_service->downloadMessagePart(id, partLocation);
}

void MailServiceWorker::downloadMessages(const QList<quint64> &msgIds)
{
    QMailMessageIdList list = from_dbus_msglist(msgIds);
    m_service->downloadMessages(list);
}

void MailServiceWorker::sendMessage(const quint64 &msgId)
{
    QMailMessageId id(msgId);
    QMailMessage message(id);
    m_service->sendMessage(message);
}

void MailServiceWorker::sendPendingMessages()
{
    m_service->sendAnyQueuedMail();
}

void MailServiceWorker::synchronizeAccount(const quint64 &accountId)
{
    QMailAccountId id(accountId);
    m_service->synchronizeAccount(id);
}

void MailServiceWorker::undoActions()
{
    m_service->undoActions();
}

void MailServiceWorker::sendAnyQueuedMail()
{
    m_service->sendAnyQueuedMail();
}

void MailServiceWorker::emptyTrash(const QList<quint64> &accountIds)
{
   QMailAccountIdList accounts = from_dbus_accountlist(accountIds);
   m_service->emptyTrash(accounts);
}

void MailServiceWorker::removeMessage(const quint64 &msgId, const int &option)
{
    QMailStore::MessageRemovalOption remOpt = static_cast<QMailStore::MessageRemovalOption>(option);
    QMailMessageId id(msgId);
    m_service->removeMessage(id, remOpt);
}

int MailServiceWorker::totalCount(const QByteArray &msgKey)
{
    return QMailStore::instance()->countMessages(to_msg_key(msgKey));
}

QList<quint64> MailServiceWorker::queryMessages(const QByteArray &msgKey, const QByteArray &sortKey, const int &limit)
{
    QMailMessageIdList result = QMailStore::instance()->queryMessages(to_msg_key(msgKey), to_msg_sort_key(sortKey), limit);
    return to_dbus_msglist(result);
}

QList<quint64> MailServiceWorker::queryFolders(const QByteArray &folderKey, const QByteArray &sortKey, const int &limit)
{
    QMailFolderIdList result = QMailStore::instance()->queryFolders(
                to_folder_key(folderKey),
                to_folder_sort_key(sortKey),
                limit, 0);
    return to_dbus_folderlist(result);
}

void MailServiceWorker::pruneCache(const QList<quint64> &msgIds)
{
    QMailMessageIdList msgs = from_dbus_msglist(msgIds);
    if (!msgs.isEmpty()) {
        QMailStore::instance()->removeMessages(QMailMessageKey::id(msgs), QMailStore::MessageRemovalOption::NoRemovalRecord);
    }
}

void MailServiceWorker::handleMessagesFetched(const QMailMessageIdList &msgIds)
{
    QList<quint64> messages = to_dbus_msglist(msgIds);
    emit messagesNowAvailable(messages);
}

void MailServiceWorker::handleMessageFetchFailed(const QMailMessageIdList &msgIds)
{
    QList<quint64> messages = to_dbus_msglist(msgIds);
    emit messageFetchFailed(messages);
}

void MailServiceWorker::handleMessagesSent(const QMailMessageIdList &msgIds)
{
    QList<quint64> messages = to_dbus_msglist(msgIds);
    emit messagesSent(messages);
}

void MailServiceWorker::handleMessageSendingFailed(const QMailMessageIdList &ids, QMailServiceAction::Status::ErrorCode error)
{
    QList<quint64> messages = to_dbus_msglist(ids);
    emit messageSendingFailed(messages, static_cast<int>(error));
}

void MailServiceWorker::handleActionFailed(const quint64 &id, const QMailServiceAction::Status &status)
{
    emit actionFailed(id, static_cast<int>(status.errorCode), status.text);
}
