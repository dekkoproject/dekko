#include "Folder.h"
#include <qmailstore.h>

Folder::Folder(QObject *parent) : QObject(parent),
    m_type(StandardFolder)
{
    connect(QMailStore::instance(), &QMailStore::folderContentsModified, this, &Folder::handleContentsModified);
}
Folder::Folder(QObject *parent, const QMailFolderId &id, const QMailMessageKey &key, const int &type):
    QObject(parent),
    m_folder(id), m_key(key), m_type((FolderType)type)
{
    connect(QMailStore::instance(), &QMailStore::folderContentsModified, this, &Folder::handleContentsModified);
}

Folder::Folder(QObject *parent, const QMailAccountId &accountId, const QMailFolderId &id, const QMailMessageKey &key, const int &type):
    QObject(parent),
    m_account(accountId), m_folder(id), m_key(key), m_type((FolderType)type)
{
    connect(QMailStore::instance(), &QMailStore::folderContentsModified, this, &Folder::handleContentsModified);
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

QString Folder::name() const
{
    return m_folder.id().isValid() ? m_folder.displayName() : QString();
}

int Folder::unreadCount()
{
    return m_folder.id().isValid() ? getUnreadCount() : 0;
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

void Folder::setFolderId(const int &id)
{
    QMailFolderId folderId(id);
    if (folderId.isValid()) {
        m_folder.setId(folderId);
        emit folderChanged();
    }
}

void Folder::handleContentsModified(const QMailFolderIdList &idList)
{
    if (idList.contains(m_folder.id())) {
        emit countChanged();
    }
}

int Folder::getUnreadCount()
{
    switch(m_type) {
    case StandardFolder:
    case SpecialUseInboxFolder:
    {
        // These actually live on the server so we don't need to keep this local
        // Now we could just call m_folder.serverUnreadCount() here but we
        // could be offline and not exported any local changes in a while so our
        // unread count might actually be less than server until the next sync happens.
        // So let's just lean on the mailstore for now.
        return QMailStore::instance()->countMessages(
                        QMailMessageKey::parentFolderId(m_folder.id()) &
                        QMailMessageKey::status(QMailMessage::Read, QMailDataComparator::Excludes)
                    );
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
        return QMailStore::instance()->countMessages(
                    QMailMessageKey::parentFolderId(m_folder.id()) &
                    m_key
                    );
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
        return QMailStore::instance()->countMessages(
                    tmpKey & // limit to this account
                    QMailMessageKey::parentFolderId(m_folder.id()) & // for folder
                    m_key & // with key
                    QMailMessageKey::status(QMailMessage::Read, QMailDataComparator::Excludes) // exclude read
                    );
    }
    case SpecialUseSentFolder:
    {
        // There shouldn't be an unread count for this.
        return 0;
    }
    }
    Q_UNREACHABLE();
}

