#include "MailServiceWorker.h"


MailServiceWorker::MailServiceWorker(QObject *parent) : QObject(parent),
    m_service(Q_NULLPTR)
{
    m_service = new ClientService(this);
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
