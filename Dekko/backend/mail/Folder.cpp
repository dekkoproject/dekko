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
#include "Folder.h"
#include <QDebug>
#include <qmailstore.h>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include "MailServiceClient.h"
#include "serviceutils.h"

Folder::Folder(QObject *parent) : QObject(parent),
    m_type(StandardFolder), m_unreadCount(0)
{
    connect(QMailStore::instance(), &QMailStore::folderContentsModified, this, &Folder::handleContentsModified);
    connect(this, &Folder::countChanged, this, &Folder::updateUnreadCount);
}
Folder::Folder(QObject *parent, const QMailFolderId &id, const QMailMessageKey &key, const int &type):
    QObject(parent),
    m_folder(id), m_key(key), m_type((FolderType)type), m_unreadCount(0)
{
    connect(QMailStore::instance(), &QMailStore::folderContentsModified, this, &Folder::handleContentsModified);
    connect(this, &Folder::countChanged, this, &Folder::updateUnreadCount);
    if (m_folder.id().isValid()) {
        updateUnreadCount();
    }
}

Folder::Folder(QObject *parent, const QMailAccountId &accountId, const QMailFolderId &id, const QMailMessageKey &key, const int &type):
    QObject(parent),
    m_account(accountId), m_folder(id), m_key(key), m_type((FolderType)type), m_unreadCount(0)
{
    connect(QMailStore::instance(), &QMailStore::folderContentsModified, this, &Folder::handleContentsModified);
    connect(this, &Folder::countChanged, this, &Folder::updateUnreadCount);
    if (m_folder.id().isValid()) {
        updateUnreadCount();
    }
}

int Folder::folderId() const
{
    return m_folder.id().toULongLong();
}

QVariant Folder::messageKey() const
{
    return m_key;
}

void Folder::setMessageKey(const QMailMessageKey &key)
{
    if (key == m_key) {
        return;
    }
    m_key = key;
    if (m_folder.id().isValid()) {
        emit folderChanged();
    }
}

Folder::FolderType Folder::type() const
{
    return m_type;
}

void Folder::setFolderType(const Folder::FolderType type)
{
    if (type == m_type) {
        return;
    }
    m_type = type;
    if (m_folder.id().isValid()) {
        emit folderChanged();
    }
}

QString Folder::name()
{
    if (!m_folder.id().isValid()) {
        return QString();
    }
    if (m_folder.id() == QMailFolder::LocalStorageFolderId
            || m_type == SpecialUseInboxFolder /* Just to get the lowercased string */
            ) {
    switch (m_type) {
    case StandardFolder:
        return m_folder.displayName();
    case SpecialUseDraftsFolder:
        return tr("Drafts");
    case SpecialUseInboxFolder:
        return tr("Inbox");
    case SpecialUseJunkFolder:
        return tr("Spam");
    case SpecialUseOutboxFolder:
        return tr("Outbox");
    case SpecialUseSentFolder:
        return tr("Sent");
    case SpecialUseTrashFolder:
        return tr("Trash");
    default:
        return m_folder.displayName();
    }
    } else {
        return m_folder.displayName();
    }
}

QString Folder::path()
{
    return m_folder.path();
}

int Folder::unreadCount()
{
    return m_unreadCount;
}

int Folder::totalCount() const
{
    return m_folder.serverCount();
}

bool Folder::syncEnabled() const
{
    return (m_folder.status() & QMailFolder::SynchronizationEnabled);
}

bool Folder::synced() const
{
    return (m_folder.status() & QMailFolder::Synchronized);
}

bool Folder::partialContent() const
{
    return (m_folder.status() & QMailFolder::PartialContent);
}

bool Folder::canCreateChildren() const
{
    return (m_folder.status() & QMailFolder::ChildCreationPermitted);
}

bool Folder::canRename() const
{
    return (m_folder.status() & QMailFolder::RenamePermitted);
}

bool Folder::canDelete() const
{
    return (m_folder.status() &QMailFolder::DeletionPermitted);
}

bool Folder::readOnly() const
{
    return (m_folder.status() & QMailFolder::ReadOnly);
}

bool Folder::isFavourite() const
{
    return (m_folder.status() & QMailFolder::Favourite);
}

int Folder::nestingDepth()
{
    if (m_type != StandardFolder) {
        // All special use folders are in the top level
        return 0;
    } else {
        QMailFolder f = m_folder;
        int depth = 0;
        while (f.parentFolderId().isValid()) {
            f = QMailFolder(f.parentFolderId());
            ++depth;
        }
        return depth;
    }
}

Folder::FolderType Folder::folderTypeFromId(const QMailFolderId &id)
{
    QMailFolder folder(id);
    if (!folder.parentAccountId().isValid() || id == QMailFolder::LocalStorageFolderId) {
        // Local folder
        return StandardFolder;
    }
    QMailAccount account(folder.parentAccountId());

    if (account.standardFolders().values().contains(id)) {
        QMailFolder::StandardFolder standardFolder = account.standardFolders().key(id);
        switch (standardFolder) {
        case QMailFolder::InboxFolder:
            return SpecialUseInboxFolder;
        case QMailFolder::OutboxFolder:
            return SpecialUseOutboxFolder;
        case QMailFolder::DraftsFolder:
            return SpecialUseDraftsFolder;
        case QMailFolder::SentFolder:
            return SpecialUseSentFolder;
        case QMailFolder::TrashFolder:
            return SpecialUseTrashFolder;
        case QMailFolder::JunkFolder:
            return SpecialUseJunkFolder;
        default:
            return StandardFolder;
        }
    }
    return StandardFolder;
}

QMailFolder::StandardFolder Folder::folderFromType(const Folder::FolderType &type)
{
    QMailFolder::StandardFolder sf = QMailFolder::InboxFolder;
    switch (type) {
    case Folder::StandardFolder:
        break;
    case Folder::SpecialUseInboxFolder:
        sf = QMailFolder::InboxFolder;
        break;
    case Folder::SpecialUseOutboxFolder:
        sf = QMailFolder::OutboxFolder;
        break;
    case Folder::SpecialUseDraftsFolder:
        sf = QMailFolder::DraftsFolder;
        break;
    case Folder::SpecialUseSentFolder:
        sf = QMailFolder::SentFolder;
        break;
    case Folder::SpecialUseTrashFolder:
        sf = QMailFolder::TrashFolder;
        break;
    case Folder::SpecialUseJunkFolder:
        sf = QMailFolder::JunkFolder;
        break;
    }
    return sf;
}

void Folder::setFolderId(const int &id)
{
    QMailFolderId folderId(id);
    if (folderId.isValid()) {
        m_folder.setId(folderId);
        updateUnreadCount();
        emit folderChanged();
    }
}

void Folder::setIsFavourite(const bool &favourite)
{
    m_folder.setStatus(QMailFolder::Favourite, favourite);
    QMailStore::instance()->updateFolder(&m_folder);
    emit folderChanged();
}

void Folder::handleContentsModified(const QMailFolderIdList &idList)
{
    if (idList.contains(m_folder.id())) {
        emit countChanged();
    }
}

void Folder::handleUnreadCount(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<int> reply = *call;
    if (reply.isError()) {
        qDebug() << "[Folder::handleUnreadCount] >> Reply error";
        return;
    }
    m_unreadCount = (int)reply.value();
    qDebug() << "{Folder::handleUnreadCount} >> GOT UNREAD COUNT";
    call->deleteLater();
    emit unreadCountChanged();
}

void Folder::updateUnreadCount()
{
    QMailMessageKey unreadKey;
    switch(m_type) {
    case StandardFolder:
    case SpecialUseInboxFolder:
    {
        // These actually live on the server so we don't need to keep this local
        // Now we could just call m_folder.serverUnreadCount() here but we
        // could be offline and not exported any local changes in a while so our
        // unread count might actually be less than server until the next sync happens.
        // So let's just lean on the mailstore for now.
        unreadKey = QMailMessageKey::parentFolderId(m_folder.id()) & QMailMessageKey::status(QMailMessage::Read, QMailDataComparator::Excludes);
        break;
    }
    case SpecialUseOutboxFolder:
    case SpecialUseDraftsFolder:
    {
        // Typically for these two the total folder count is always shown
        // Mail waiting to be sent or is still a draft doesn't have a concept of unread vs read
        QMailMessageKey tmpKey;
        // If this Folder belongs to an account then we want to filter out any messages
        //belonging to other accounts. Since these local folders are shared.
        if (m_account.isValid() && m_folder.id() == QMailFolder::LocalStorageFolderId) {
            tmpKey =  QMailMessageKey::parentAccountId(m_account);
        }
        unreadKey = QMailMessageKey::parentFolderId(m_folder.id()) & m_key;
        break;
     }
    case SpecialUseJunkFolder:
    case SpecialUseTrashFolder:
    {
        // Typically for these two the total folder count is always shown
        // Mail waiting to be sent or is still a draft doesn't have a concept of unread vs read
        QMailMessageKey tmpKey;
        // If this Folder belongs to an account then we want to filter out any messages
        //belonging to other accounts. Since these local folders are shared.
        if (m_account.isValid() && m_folder.id() == QMailFolder::LocalStorageFolderId) {
            tmpKey =  QMailMessageKey::parentAccountId(m_account);
        }
        unreadKey = tmpKey & // limit to this account
                    QMailMessageKey::parentFolderId(m_folder.id()) & // for folder
                    m_key & // with key
                    QMailMessageKey::status(QMailMessage::Read, QMailDataComparator::Excludes);
        break;
    }
    case SpecialUseSentFolder:
    {
        // There shouldn't be an unread count for this.
        return;
    }
    }

    QDBusPendingReply<int> reply = Client::instance()->bus()->totalCount(msg_key_bytes(unreadKey));

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &Folder::handleUnreadCount);
}

