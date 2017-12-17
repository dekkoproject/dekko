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
#ifndef FOLDER_H
#define FOLDER_H

#include <QObject>
#include <QDBusPendingCallWatcher>
#include <qmailfolder.h>
#include <qmailmessagekey.h>

class Folder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int folderId READ folderId WRITE setFolderId NOTIFY folderChanged)
    Q_PROPERTY(QVariant messageKey READ messageKey NOTIFY folderChanged)
    Q_PROPERTY(FolderType type READ type NOTIFY folderChanged)
    Q_PROPERTY(QString name READ name NOTIFY folderChanged)
    Q_PROPERTY(QString path READ path NOTIFY folderChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY countChanged)
    Q_PROPERTY(bool synchronizationEnabled READ syncEnabled NOTIFY folderChanged)
    Q_PROPERTY(bool isSynchronized READ synced NOTIFY folderChanged)
    Q_PROPERTY(bool partialContent READ partialContent NOTIFY folderChanged)
    Q_PROPERTY(bool canCreateChildren READ canCreateChildren NOTIFY folderChanged)
    Q_PROPERTY(bool canRename READ canRename NOTIFY folderChanged)
    Q_PROPERTY(bool canDelete READ canDelete NOTIFY folderChanged)
    Q_PROPERTY(bool isReadOnly READ readOnly NOTIFY folderChanged)
    Q_PROPERTY(bool isFavourite READ isFavourite WRITE setIsFavourite NOTIFY folderChanged)
    Q_PROPERTY(int nestingDepth READ nestingDepth NOTIFY folderChanged)
    Q_ENUMS(FolderType)

public:
    explicit Folder(QObject *parent = 0);
    Folder(QObject *parent, const QMailFolderId &id,
           const QMailMessageKey &key, const int &type);
    // Folder belonging to a single account
    Folder(QObject *parent, const QMailAccountId &accountId,
           const QMailFolderId &id, const QMailMessageKey &key, const int &type);

    enum FolderType {
        StandardFolder,
        SpecialUseInboxFolder,
        SpecialUseOutboxFolder,
        SpecialUseDraftsFolder,
        SpecialUseSentFolder,
        SpecialUseTrashFolder,
        SpecialUseJunkFolder
    };

    int folderId() const;
    QMailFolderId id() const { return m_folder.id(); }
    QVariant messageKey() const;
    void setMessageKey(const QMailMessageKey &key);
    FolderType type() const;
    void setFolderType(const FolderType type);
    QString name();
    QString path();
    int unreadCount();
    int totalCount() const;
    bool syncEnabled() const;
    bool synced() const;
    bool partialContent() const;
    bool canCreateChildren() const;
    bool canRename() const;
    bool canDelete() const;
    bool readOnly() const;
    bool isFavourite() const;
    int nestingDepth();

    // Helpers for FolderList
    void emitFolderChanged() {
        emit folderChanged();
    }
    void emitCountChanged() {
        emit countChanged();
    }

    static FolderType folderTypeFromId(const QMailFolderId &id);
    static QMailFolder::StandardFolder folderFromType(const FolderType &type);

signals:
    void folderChanged();
    void countChanged();
    void unreadCountChanged();

public slots:
    void setFolderId(const int &id);
    void setIsFavourite(const bool &favourite);
    void handleContentsModified(const QMailFolderIdList &idList);

private slots:
    void updateUnreadCount();
    void handleUnreadCount(QDBusPendingCallWatcher *call);

private:
    QMailAccountId m_account;
    QMailFolder m_folder;
    QMailMessageKey m_key;
    FolderType m_type;
    int m_unreadCount;
};
#endif // FOLDER_H
