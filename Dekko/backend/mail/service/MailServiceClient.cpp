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
#include <MailServiceClient.h>
#include <QDebug>
#include <QPointer>
#include <qmailstore.h>
#include <QDBusConnection>
#include "MailServiceWorker.h"
#include "serviceutils.h"

#define SERVICE "org.dekkoproject.Service"
#define SERVICE_PATH "/mail"

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
    m_service(0),
    m_mService(0)
{

    MailServiceWorker::registerTypes();

    m_mService = new MailServiceInterface(SERVICE, SERVICE_PATH, QDBusConnection::sessionBus());

    m_service = new ClientService(this);
    emit serviceChanged();

    connect(m_mService, &MailServiceInterface::messagePartNowAvailable, this, &Client::messagePartNowAvailable);
    connect(m_mService, &MailServiceInterface::messagePartFetchFailed, this, &Client::messagePartFetchFailed);
    connect(m_mService, &MailServiceInterface::messagesNowAvailable, this, &Client::handleMessagesNowAvailable);
    connect(m_mService, &MailServiceInterface::messageFetchFailed, this, &Client::handleMessageFetchFailed);
    connect(m_mService, &MailServiceInterface::messagesSent, this, &Client::handleMessagesSent);
    connect(m_mService, &MailServiceInterface::messageSendingFailed, this, &Client::handleMessageSendingFailed);
    connect(m_mService, &MailServiceInterface::accountSynced, this, &Client::accountSynced);
    connect(m_mService, &MailServiceInterface::syncAccountFailed, this, &Client::syncAccountFailed);
    connect(m_mService, &MailServiceInterface::standardFoldersCreated, this, &Client::standardFoldersCreated);
    connect(m_mService, &MailServiceInterface::actionFailed, this, &Client::handleFailure);
    connect(m_mService, &MailServiceInterface::undoCountChanged, this, &Client::undoCountChanged);
}

bool Client::hasUndoableActions() const
{
    return m_mService->hasUndoableAction();
}

QString Client::undoDescription() const
{
    return m_mService->undoDescription();
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
    m_mService->deleteMessages(to_dbus_msglist(idList));
}

void Client::restoreMessage(const int &msgId)
{
    m_mService->restoreMessage(QMailMessageId(msgId).toULongLong());
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
    m_mService->markMessagesImportant(to_dbus_msglist(idList), important);
}

void Client::markMessagesRead(const QMailMessageIdList &idList, const bool read)
{
    m_mService->markMessagesRead(to_dbus_msglist(idList), read);
}

void Client::markMessagesTodo(const QMailMessageIdList &idList, const bool todo)
{
    m_mService->markMessagesTodo(to_dbus_msglist(idList), todo);
}

void Client::markMessagesDone(const QMailMessageIdList &idList, const bool done)
{
    m_mService->markMessagesDone(to_dbus_msglist(idList), done);
}

void Client::createStandardFolders(const quint64 &accountId)
{
    createStandardFolders(QMailAccountId(accountId));
}

void Client::createStandardFolders(const QMailAccountId &accountId)
{
    m_mService->createStandardFolders(accountId.toULongLong());
}

void Client::markMessagesReplied(const QMailMessageIdList &idList, const bool all)
{
    m_mService->markMessagesReplied(to_dbus_msglist(idList), all);
}

void Client::markMessageForwarded(const QMailMessageIdList &idList)
{
    m_mService->markMessageForwarded(to_dbus_msglist(idList));
}

void Client::markFolderRead(const QMailFolderId &id)
{
    m_mService->markFolderRead(id.toULongLong());
}

void Client::emptyTrash(const QMailAccountIdList &ids)
{
    m_mService->emptyTrash(to_dbus_accountlist(ids));
}

void Client::syncFolders(const QMailAccountId &accountId, const QMailFolderIdList &folders)
{
    m_mService->syncFolders(accountId.toULongLong(), to_dbus_folderlist(folders));
}

void Client::downloadMessagePart(const QMailMessagePart *msgPart)
{
    qDebug() << "[Client]" << "Downloading message part" << msgPart->location().toString(true);
    quint64 id = msgPart->location().containingMessageId().toULongLong();
    QString location = msgPart->location().toString(true);
    m_mService->downloadMessagePart(id, location);
}

void Client::downloadMessage(const QMailMessageId &msgId)
{
    downloadMessages(QMailMessageIdList() << msgId);
}

void Client::downloadMessages(const QMailMessageIdList &idList)
{
    Q_UNUSED(idList);
}

void Client::synchronizeAccount(const QMailAccountId &id)
{
    m_mService->synchronizeAccount(id.toULongLong());
}

bool Client::addMessage(QMailMessage *msg)
{
    return QMailStore::instance()->addMessage(msg);
}

bool Client::removeMessage(const QMailMessageId &id, const QMailStore::MessageRemovalOption &option)
{
    m_mService->removeMessage(id.toULongLong(), static_cast<int>(option));
    return true;
}

void Client::moveToStandardFolder(const QMailMessageIdList &msgIds, const Folder::FolderType &folder, const bool userTriggered)
{
    m_mService->moveToStandardFolder(to_dbus_msglist(msgIds), static_cast<int>(folder), userTriggered);
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
    m_mService->moveToFolder(to_dbus_msglist(ids), folderId.toULongLong());
}

void Client::sendMessage(const QMailMessage &msg)
{
    m_mService->sendMessage(msg.id().toULongLong());
}

void Client::sendPendingMessages()
{
    m_mService->sendPendingMessages();
}

void Client::undoActions()
{
    m_mService->undoActions();
}

void Client::handleFailure(const quint64 &id, const int &statusCode, const QString &statusText)
{
    QMailServiceAction::Status::ErrorCode s = static_cast<QMailServiceAction::Status::ErrorCode>(statusCode);
    Error error;
    switch(s) {
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
    emit clientError(id, error, statusText);
}

void Client::handleMessagesNowAvailable(const QList<quint64> &msgIds)
{
    QMailMessageIdList messages = from_dbus_msglist(msgIds);
    emit messagesNowAvailable(messages);
}

void Client::handleMessageFetchFailed(const QList<quint64> &msgIds)
{
    QMailMessageIdList messages = from_dbus_msglist(msgIds);
    emit messageFetchFailed(messages);
}

void Client::handleMessagesSent(const QList<quint64> &msgIds)
{
    QMailMessageIdList messages = from_dbus_msglist(msgIds);
    emit messagesSent(messages);
}

void Client::handleMessageSendingFailed(const QList<quint64> &msgIds, const int &error)
{
    QMailMessageIdList messages = from_dbus_msglist(msgIds);
    QMailServiceAction::Status::ErrorCode err = static_cast<QMailServiceAction::Status::ErrorCode>(error);
    emit messageSendingFailed(messages, err);
}

QMailAccountIdList Client::getEnabledAccountIds() const
{
    return QMailStore::instance()->queryAccounts(QMailAccountKey::messageType(QMailMessage::Email)
                                                 & QMailAccountKey::status(QMailAccount::Enabled),
                                                 QMailAccountSortKey::name());
}

