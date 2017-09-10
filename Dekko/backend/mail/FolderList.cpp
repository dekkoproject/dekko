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
#include "FolderList.h"
#include <algorithm>
#include <qmailfolder.h>
#include <qmailfolderkey.h>
#include <qmailstore.h>

FolderList::FolderList(QObject *parent) : QObject(parent), m_model(0)
{
    m_model = new QQmlObjectListModel<Folder>(this);
    emit modelChanged();
    connect(this, &FolderList::accountIdChanged, this, &FolderList::reset);
    reset();
}

void FolderList::reset()
{
    m_model->clear();
    QMailFolderKey folderKey = QMailFolderKey::parentAccountId(m_accountId);
    QMailFolderIdList folderList = QMailStore::instance()->queryFolders(folderKey);
    std::sort(folderList.begin(), folderList.end(), FolderList::sortIdList);

    QMailMessageKey excludeKey = QMailMessageKey::status(QMailMessage::Removed, QMailDataComparator::Excludes);
    QMailAccount account(m_accountId);

    QMailFolderId inboxId = account.standardFolder(QMailFolder::InboxFolder);
    addFolder(inboxId, excludeKey, folderList);

    QMailFolderId draftsId = account.standardFolder(QMailFolder::DraftsFolder);
    if (!draftsId.isValid()) {
        insertFolder(QMailFolder::LocalStorageFolderId, Folder::SpecialUseDraftsFolder,
                     QMailMessageKey::status(QMailMessage::Draft) &
                     ~QMailMessageKey::status(QMailMessage::Outbox) &
                     ~QMailMessageKey::status(QMailMessage::Trash) &
                     excludeKey);
    } else {
        addFolder(draftsId, excludeKey, folderList);
    }

    QMailFolderId sentFolderId = account.standardFolder(QMailFolder::SentFolder);
    if (!sentFolderId.isValid()) {
        insertFolder(QMailFolder::LocalStorageFolderId, Folder::SpecialUseSentFolder,
                               QMailMessageKey::status(QMailMessage::Sent) &
                               ~QMailMessageKey::status(QMailMessage::Trash) &
                               excludeKey);
    } else {
        addFolder(sentFolderId, excludeKey, folderList);
    }

    // Take trash and childs
    QMailFolderId trashFolderId = account.standardFolder(QMailFolder::TrashFolder);
    if (!trashFolderId.isValid()) {
        insertFolder(QMailFolder::LocalStorageFolderId, Folder::SpecialUseTrashFolder,
                               QMailMessageKey::status(QMailMessage::Trash) &
                               excludeKey);
    } else {
        addFolder(trashFolderId, excludeKey, folderList);
    }

    insertFolder(QMailFolder::LocalStorageFolderId, Folder::SpecialUseOutboxFolder,
                                          QMailMessageKey::status(QMailMessage::Outbox) &
                                          ~QMailMessageKey::status(QMailMessage::Trash) &
                                          excludeKey);
    // Add the remaining folders, they are already ordered
    foreach (const QMailFolderId& folderId, folderList) {
        Folder::FolderType folderType = Folder::folderTypeFromId(folderId);
        if (folderType != Folder::SpecialUseTrashFolder) {
            excludeKey &= QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes);
        }
        insertFolder(folderId, folderType, excludeKey);
    }
}

void FolderList::setAccountId(const quint64 &id)
{
    m_accountId = QMailAccountId(id);
    if (QMailAccountId(id).isValid()) {
        m_accountId = QMailAccountId(id);
        emit accountIdChanged();
    }
}

void FolderList::addFolder(const QMailFolderId &folderId, QMailMessageKey key, QMailFolderIdList &folderList)
{
    Folder::FolderType fType = Folder::folderTypeFromId(folderId);
    auto isStandardFolder = [](const Folder::FolderType &folderType) {
        return folderType == Folder::SpecialUseInboxFolder || folderType == Folder::SpecialUseDraftsFolder
                    || folderType == Folder::SpecialUseJunkFolder || folderType == Folder::SpecialUseTrashFolder
                    || folderType == Folder::SpecialUseOutboxFolder || folderType == Folder::SpecialUseSentFolder;
    };

    int i = folderList.indexOf(folderId);
    if (i != -1) {
        insertFolder(folderList[i], fType, key);
        folderList.removeAt(i);
        int n = i;
        while (n < folderList.size() && QMailFolder(folderList[n]).parentFolderId() == folderId) {
            if (isStandardFolder(fType)) {
                n++;
            } else {
                Folder::FolderType folderType = Folder::folderTypeFromId(folderList[n]);
                if (folderType != Folder::SpecialUseTrashFolder) {
                    key &= QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes);
                }
                insertFolder(folderList[n], folderType, key);
                folderList.removeAt(n);
            }
        }
    }
}

void FolderList::insertFolder(const QMailFolderId &id, const Folder::FolderType &type, const QMailMessageKey &key)
{
    QMailMessageKey msgKey = QMailMessageKey::parentFolderId(id) & key;
    Folder *f = new Folder(0, id, msgKey, type);
    m_model->append(f);
}

bool FolderList::sortIdList(const QMailFolderId &a, const QMailFolderId &b)
{
    QMailFolder folderA(a), folderB(b);
    if (folderA.parentFolderId() == folderB.parentFolderId()) {
        // check they have different names
        return folderA.displayName().compare(folderB.displayName(), Qt::CaseInsensitive) < 0;
    } else if (folderA.parentAccountId() != folderB.parentAccountId()) {
        return folderA.parentAccountId() < folderB.parentAccountId();
    } else {
        QMailFolderIdList aParents;
        QMailFolderId pId = a;
        while (pId.isValid()) {
            if (!(QMailFolder(pId).status() & QMailFolder::NonMail)) {
                aParents.append(pId);
            }
            pId = QMailFolder(pId).parentFolderId();
        }
        if (aParents.contains(b)) {
            return false;
        }
        QMailFolderId cId;
        QMailFolderId bLP;
        pId = b;
        while (pId.isValid()) {
            if (aParents.contains(pId)) {
                cId = pId;
                break;
            }
            QMailFolder folderB(pId);
            if (!(folderB.status() & QMailFolder::NonMail)) {
                bLP = pId;
            }
            pId = folderB.parentFolderId();
        }

        if (cId.isValid()) {
            int idIsParentOfA = aParents.indexOf(cId);
            if (idIsParentOfA == 0) {
                // a is ancestor of b
                return true;
            }
            // Common ancestor found
            return sortIdList(aParents[idIsParentOfA - 1], bLP);
        } else {
            QMailFolder topA(aParents.last()), topB(bLP);
            // No common ancestor found
            return topA.displayName().compare(topB.displayName(), Qt::CaseInsensitive) < 0;
        }
    }
}
