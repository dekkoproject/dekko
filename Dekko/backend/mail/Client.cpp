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
#include "Client.h"
#include <QDebug>
#include <QPointer>
#include <qmailstore.h>

static QPointer<Client> s_client;
Client *Client::instance()
{
    if (s_client.isNull()) {
        s_client = new Client();
    }
    return s_client;
}

QObject *Client::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return Client::instance();
}

Client::Client(QObject *parent) : QObject(parent),
    m_service(0)
{
    m_service = new ClientService(this);
    emit serviceChanged();
    connect(m_service, &ClientService::messagePartFetched, this, &Client::messagePartNowAvailable);
    connect(m_service, &ClientService::messagePartFetchFailed, this, &Client::messagePartFetchFailed);
    connect(m_service, &ClientService::messagesFetched, this, &Client::messagesNowAvailable);
    connect(m_service, &ClientService::messageFetchFailed, this, &Client::messageFetchFailed);
    connect(m_service, &ClientService::messagesSent, this, &Client::messagesSent);
    connect(m_service, &ClientService::messageSendingFailed, this, &Client::messageSendingFailed);
    connect(m_service, &ClientService::accountSynced, this, &Client::accountSynced);
    connect(m_service, &ClientService::syncAccountFailed, this, &Client::syncAccountFailed);
    connect(m_service, &ClientService::actionFailed, this, &Client::handleFailure);
    connect(m_service, &ClientService::standardFoldersCreated, this, &Client::standardFoldersCreated);
}

bool Client::hasConfiguredAccounts()
{
    return QMailStore::instance()->countAccounts() > 0;
}

void Client::deleteMessage(const int &msgId)
{
    qDebug() << "Msg id to delete is: " << msgId;
    deleteMessages(QMailMessageIdList() << QMailMessageId(msgId));
}

void Client::deleteMessages(const QMailMessageIdList &idList)
{
    m_service->deleteMessages(idList);
}

void Client::restoreMessage(const int &msgId)
{
    m_service->restoreMessage(QMailMessageId(msgId));
}

void Client::markMessageImportant(const int &msgId, const bool important)
{
    markMessagesImportant(QMailMessageIdList() << QMailMessageId(msgId), important);
}

void Client::markMessageRead(const int &msgId, const bool read)
{
    markMessagesRead(QMailMessageIdList() << QMailMessageId(msgId), read);
}

void Client::markMessageTodo(const int &msgId, const bool todo)
{
    markMessagesTodo(QMailMessageIdList() << QMailMessageId(msgId), todo);
}

void Client::markMessageDone(const int &msgId, const bool done)
{
    markMessagesDone(QMailMessageIdList() << QMailMessageId(msgId), done);
}

void Client::moveToStandardFolder(const int &msgId, const int &standardFolder)
{
    moveToStandardFolder(QMailMessageIdList() << QMailMessageId(msgId), (Folder::FolderType)standardFolder);
}

void Client::markStandardFolderRead(const int &standardFolder)
{
    Q_FOREACH(auto &id, getEnabledAccountIds()) {
        markStandardFolderRead(id.toULongLong(), standardFolder);
    }
}

void Client::markStandardFolderRead(const quint64 &accountId, const int &standardFolder)
{
    QMailAccountId id(accountId);
    QMailAccount account(id);
    QMailFolderId folder = account.standardFolder(Folder::folderFromType((Folder::FolderType)standardFolder));
    markFolderRead(folder);
}

void Client::synchronizeAccount(const int &id)
{
    synchronizeAccount(QMailAccountId(id));
}

void Client::syncStandardFolder(const int &standardFolder)
{
    Q_FOREACH(auto &id, getEnabledAccountIds()) {
        syncStandardFolder(id.toULongLong(), standardFolder);
    }
}

void Client::syncStandardFolder(const quint64 &accountId, const int &standardFolder)
{
    QMailAccountId id(accountId);
    QMailAccount account(id);
    QMailFolderId folder = account.standardFolder(Folder::folderFromType((Folder::FolderType)standardFolder));
    syncFolders(id, QMailFolderIdList() << folder);
}

void Client::syncFolder(const quint64 &accountId, const quint64 &folderId)
{
    QMailAccountId id(accountId);
    QMailFolderId folder(folderId);
    syncFolders(id, QMailFolderIdList() << folder);
}

void Client::emptyTrash()
{
    emptyTrash(getEnabledAccountIds());
}

void Client::emptyTrash(const int &accountId)
{
    emptyTrash(QMailAccountIdList() << QMailAccountId(accountId));
}

void Client::markMessagesImportant(const QMailMessageIdList &idList, const bool important)
{
    m_service->markMessagesImportant(idList, important);
}

void Client::markMessagesRead(const QMailMessageIdList &idList, const bool read)
{
    m_service->markMessagesRead(idList, read);
}

void Client::markMessagesTodo(const QMailMessageIdList &idList, const bool todo)
{
    m_service->markMessagesTodo(idList, todo);
}

void Client::markMessagesDone(const QMailMessageIdList &idList, const bool done)
{
    m_service->markMessagesDone(idList, done);
}

void Client::createStandardFolders(const quint64 &accountId)
{
    m_service->createStandardFolders(QMailAccountId(accountId));
}

void Client::markMessagesReplied(const QMailMessageIdList &idList, const bool all)
{
    m_service->markMessagesReplied(idList, all);
}

void Client::markMessageForwarded(const QMailMessageIdList &idList)
{
    m_service->markMessageForwarded(idList);
}

void Client::markFolderRead(const QMailFolderId &id)
{
    m_service->markFolderRead(id);
}

void Client::emptyTrash(const QMailAccountIdList &ids)
{
    m_service->emptyTrash(ids);
}

void Client::syncFolders(const QMailAccountId &accountId, const QMailFolderIdList &folders)
{
    m_service->syncFolders(accountId, folders);
}

void Client::downloadMessagePart(const QMailMessagePart *msgPart)
{
    qDebug() << "[Client]" << "Downloading message part" << msgPart->location().toString(true);
    m_service->downloadMessagePart(msgPart);
}

void Client::downloadMessage(const QMailMessageId &msgId)
{
    downloadMessages(QMailMessageIdList() << msgId);
}

void Client::downloadMessages(const QMailMessageIdList &idList)
{

}

void Client::synchronizeAccount(const QMailAccountId &id)
{
    m_service->synchronizeAccount(id);
}

bool Client::addMessage(QMailMessage *msg)
{
    return QMailStore::instance()->addMessage(msg);
}

bool Client::removeMessage(const QMailMessageId &id, const QMailStore::MessageRemovalOption &option)
{
    return QMailStore::instance()->removeMessage(id, option);
}

void Client::moveToStandardFolder(const QMailMessageIdList &msgIds, const Folder::FolderType &folder, const bool userTriggered)
{
    m_service->moveToStandardFolder(msgIds, folder, userTriggered);
}

void Client::moveToFolder(const quint64 &msgId, const quint64 &folderId)
{
    QMailMessageId id(msgId);
    QMailFolderId folder(folderId);

    if (!id.isValid() || !folder.isValid()) {
        return;
    }
    moveToFolder(QMailMessageIdList() << id, folder);
}

void Client::moveToFolder(const QMailMessageIdList &ids, const QMailFolderId &folderId)
{
    m_service->moveToFolder(ids, folderId);
}

void Client::sendMessage(const QMailMessage &msg)
{
    m_service->sendMessage(msg);
}

void Client::sendPendingMessages()
{
    m_service->sendAnyQueuedMail();
}

void Client::handleFailure(const quint64 &id, const QMailServiceAction::Status &status)
{
    Error error;
    switch(status.errorCode) {
    case QMailServiceAction::Status::ErrNoError:
        error = Error::NoError;
        break;
    case QMailServiceAction::Status::ErrNotImplemented:
        error = Error::UnknownError;
        break;
    case QMailServiceAction::Status::ErrFrameworkFault:
        error = Error::FrameworkFault;
        break;
    case QMailServiceAction::Status::ErrSystemError:
        error = Error::SystemError;
        break;
    case QMailServiceAction::Status::ErrInternalServer:
        error = Error::InternalServerError;
        break;
    case QMailServiceAction::Status::ErrUnknownResponse:
        error = Error::UnexpectedResponse;
        break;
    case QMailServiceAction::Status::ErrLoginFailed:
        error = Error::LoginFailed;
        break;
    case QMailServiceAction::Status::ErrCancel:
        error = Error::CancelError;
        break;
    case QMailServiceAction::Status::ErrFileSystemFull:
        error = Error::FileSystemFull;
        break;
    case QMailServiceAction::Status::ErrNonexistentMessage:
        error = Error::MessageNotExist;
        break;
    case QMailServiceAction::Status::ErrEnqueueFailed:
        error = Error::EnqueueFailed;
        break;
    case QMailServiceAction::Status::ErrNoConnection:
        error = Error::NoConnection;
        break;
    case QMailServiceAction::Status::ErrConnectionInUse:
        error = Error::ConnectionInUse;
        break;
    case QMailServiceAction::Status::ErrConnectionNotReady:
        error = Error::ConnectionNotReady;
        break;
    case QMailServiceAction::Status::ErrConfiguration:
        error = Error::ConfigurationError;
        break;
    case QMailServiceAction::Status::ErrInvalidAddress:
        error = Error::InvalidAddress;
        break;
    case QMailServiceAction::Status::ErrInvalidData:
        error = Error::InvalidData;
        break;
    case QMailServiceAction::Status::ErrTimeout:
        error = Error::TimeoutError;
        break;
    case QMailServiceAction::Status::ErrInternalStateReset:
        error = Error::InternalStateReset;
        break;
    case QMailServiceAction::Status::ErrNoSslSupport:
        error = Error::SslNotSupported;
        break;
    case QMailServiceAction::Status::ErrUntrustedCertificates:
        error = Error::UntrustedCertificate;
        break;
        error = Error::UnknownError;
        break;
    }
    emit clientError(id, error, status.text);
}

QMailAccountIdList Client::getEnabledAccountIds() const
{
    return QMailStore::instance()->queryAccounts(QMailAccountKey::messageType(QMailMessage::Email)
                                                 & QMailAccountKey::status(QMailAccount::Enabled),
                                                 QMailAccountSortKey::name());
}
