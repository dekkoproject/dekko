#include "MailServiceWorker.h"
#include <QDataStream>
#include <QByteArray>
#include <qmailstore.h>


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
    QMailMessageIdList mIds = toMsgIdList(ids);
    m_service->deleteMessages(mIds);
}

void MailServiceWorker::restoreMessage(const quint64 &id)
{
    QMailMessageId mid(id);
    m_service->restoreMessage(mid);
}

void MailServiceWorker::markMessagesImportant(const QList<quint64> &msgIds, const bool important)
{
    QMailMessageIdList mIds = toMsgIdList(msgIds);
    m_service->markMessagesImportant(mIds, important);
}

void MailServiceWorker::markMessagesRead(const QList<quint64> &msgIds, const bool read)
{
    qDebug() << "Marking Message Read: " << msgIds;
    QMailMessageIdList mIds = toMsgIdList(msgIds);
    m_service->markMessagesRead(mIds, read);
}

void MailServiceWorker::markMessagesTodo(const QList<quint64> &msgIds, const bool todo)
{
    QMailMessageIdList mIds = toMsgIdList(msgIds);
    m_service->markMessagesTodo(mIds, todo);
}

void MailServiceWorker::markMessagesDone(const QList<quint64> &msgIds, const bool done)
{
    QMailMessageIdList mIds = toMsgIdList(msgIds);
    m_service->markMessagesDone(mIds, done);
}

void MailServiceWorker::markMessagesReplied(const QList<quint64> &msgIds, const bool all)
{
    QMailMessageIdList list = toMsgIdList(msgIds);
    m_service->markMessagesReplied(list, all);
}

void MailServiceWorker::markMessageForwarded(const QList<quint64> &msgIds)
{
    QMailMessageIdList list = toMsgIdList(msgIds);
    m_service->markMessageForwarded(list);
}

void MailServiceWorker::syncFolders(const quint64 &accountId, const QList<quint64> &folders)
{
    QMailAccountId id(accountId);
    QMailFolderIdList list = toFolderIdList(folders);
    m_service->syncFolders(id, list);
}

void MailServiceWorker::createStandardFolders(const quint64 &accountId)
{
    QMailAccountId id(accountId);
    m_service->createStandardFolders(id);
}

void MailServiceWorker::moveToFolder(const QList<quint64> &msgIds, const quint64 &folderId)
{
    QMailMessageIdList list = toMsgIdList(msgIds);
    QMailFolderId id(folderId);
    m_service->moveToFolder(list, id);
}

void MailServiceWorker::moveToStandardFolder(const QList<quint64> &msgIds, const int &folder, const bool userTriggered)
{
    QMailMessageIdList list = toMsgIdList(msgIds);
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
    QMailMessageIdList list = toMsgIdList(msgIds);
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
   QMailAccountIdList accounts = toAccountIdList(accountIds);
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
    return QMailStore::instance()->countMessages(toMessageKey(msgKey));
}

QList<quint64> MailServiceWorker::queryMessages(const QByteArray &msgKey, const QByteArray &sortKey, const int &limit)
{
    QMailMessageIdList result = QMailStore::instance()->queryMessages(toMessageKey(msgKey), toMessageSortKey(sortKey), limit);
    return toDBusMsgIdList(result);
}

void MailServiceWorker::handleMessagesFetched(const QMailMessageIdList &msgIds)
{
    QList<quint64> messages = toDBusMsgIdList(msgIds);
    emit messagesNowAvailable(messages);
}

void MailServiceWorker::handleMessageFetchFailed(const QMailMessageIdList &msgIds)
{
    QList<quint64> messages = toDBusMsgIdList(msgIds);
    emit messageFetchFailed(messages);
}

void MailServiceWorker::handleMessagesSent(const QMailMessageIdList &msgIds)
{
    QList<quint64> messages = toDBusMsgIdList(msgIds);
    emit messagesSent(messages);
}

void MailServiceWorker::handleMessageSendingFailed(const QMailMessageIdList &ids, QMailServiceAction::Status::ErrorCode error)
{
    QList<quint64> messages = toDBusMsgIdList(ids);
    emit messageSendingFailed(messages, static_cast<int>(error));
}

void MailServiceWorker::handleActionFailed(const quint64 &id, const QMailServiceAction::Status &status)
{
    emit actionFailed(id, static_cast<int>(status.errorCode), status.text);
}

QMailMessageKey MailServiceWorker::toMessageKey(const QByteArray &key)
{
    QMailMessageKey mKey;
    QByteArray m = key;
    QDataStream keystream(&m, QIODevice::ReadWrite);
    mKey.deserialize<QDataStream>(keystream);
    return mKey;
}

QMailMessageSortKey MailServiceWorker::toMessageSortKey(const QByteArray &key)
{
    QMailMessageSortKey sKey;
    QByteArray s = key;
    QDataStream skeystream(&s, QIODevice::ReadWrite);
    sKey.deserialize(skeystream);
    return sKey;
}

QMailMessageIdList MailServiceWorker::toMsgIdList(const QList<quint64> &ids)
{
    QMailMessageIdList list;
    foreach(const quint64 &id, ids) {
        list << QMailMessageId(id);
    }
    return list;
}

QMailFolderIdList MailServiceWorker::toFolderIdList(const QList<quint64> &ids)
{
    QMailFolderIdList list;
    foreach(const quint64 &id, ids) {
        list << QMailFolderId(id);
    }
    return list;
}

QMailAccountIdList MailServiceWorker::toAccountIdList(const QList<quint64> &ids)
{
    QMailAccountIdList accounts;
    foreach(const quint64 &id, ids) {
        QMailAccountId account(id);
        accounts << account;
    }
    return accounts;
}

QList<quint64> MailServiceWorker::toDBusMsgIdList(const QMailMessageIdList &msgIds) {
    QList<quint64> list;
    foreach(const QMailMessageId &id, msgIds) {
        list << id.toULongLong();
    }
    return list;
}
