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
#ifndef MESSAGESET_H
#define MESSAGESET_H

#include <QObject>
#include <QDBusPendingCallWatcher>
#include <QFlags>
#include <QmlObjectListModel.h>
#include <qmailmessagekey.h>
#include <qmailfolder.h>
#include <QTimer>

class MessageSet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hasDescendents READ hasDecendents NOTIFY descendentsCountChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QObject *descendents READ children CONSTANT)
    Q_PROPERTY(QVariant messageKey READ messageKey NOTIFY messageKeyChanged)
    Q_PROPERTY(QVariant descendentsKey READ descendentsKey NOTIFY descendentsCountChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(SupportedActions supportedActions READ supportedActions CONSTANT)

public:
    explicit MessageSet(QObject *parent = 0);

    enum SupportedAction {
        NoActions = 0,
        SyncAction = 1 << 0,
        MarkAsRead = 1 << 1,
        ExportPending = 1 << 2,
        Expunge = 1 << 3,
        RenameAllowed = 1 << 4,
        SearchFolder = 1 << 5,
        ShowProperties = 1 << 6,
        MarkAsDone = 1 << 7, // for the Todo Items
        EmptyTrash = 1 << 8
    };

    Q_DECLARE_FLAGS(SupportedActions, SupportedAction)
    Q_FLAGS(SupportedActions)
    Q_ENUMS(SupportedAction)

    virtual void init(const QString &displayName, const QMailMessageKey &messageKey) = 0;
    QObject *children() { return m_children; }
    QVariant messageKey() const { return m_key; }
    virtual QVariant descendentsKey() = 0;
    QString displayName() const { return m_name; }
    bool hasDecendents() const { return !m_children->isEmpty(); }
    int unreadCount();
    int totalCount();
    SupportedActions supportedActions() const { return m_actions; }
    void setSupportedActions(const SupportedActions &actions) { m_actions = actions; }

signals:
    void messageKeyChanged();
    void descendentsCountChanged();
    void displayNameChanged();
    void countChanged();
    void unreadCountChanged();
    void totalCountChanged();

public slots:
    void setMessageKey(const QMailMessageKey &key);
    void setDisplayName(const QString &displayName);
    void updateCounts();

private slots:
    void updateUnreadCount(QDBusPendingCallWatcher *call);
    void updateTotalCount(QDBusPendingCallWatcher *call);

protected:
    QString m_name;
    QQmlObjectListModel<MessageSet> *m_children;
    QMailMessageKey m_key;
    SupportedActions m_actions;
    int m_unreadCount;
    int m_totalCount;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MessageSet::SupportedActions)


class StandardFolderSet : public MessageSet
{
    Q_OBJECT
    Q_PROPERTY(FolderType folderType READ type WRITE setType NOTIFY standardFolderChanged)
    Q_ENUMS(FolderType)
public:
    explicit StandardFolderSet(QObject *parent = 0);
    // Kind of annoying we have to duplicate this here for QML :-(
    // But i much prefer declaring the type in the qproperty rather than just an int.
    enum FolderType {
        StandardFolder,
        SpecialUseInboxFolder,
        SpecialUseOutboxFolder,
        SpecialUseDraftsFolder,
        SpecialUseSentFolder,
        SpecialUseTrashFolder,
        SpecialUseJunkFolder
    };
    virtual void init(const QString &displayName, const QMailMessageKey &messageKey);
    void initNoDecendents(const QString &displayName, const QMailMessageKey &messageKey);
    virtual QVariant descendentsKey() override;
    FolderType type() const { return m_type; }

signals:
    void standardFolderChanged();

public slots:
    void setType(FolderType type);

private slots:
    // initiate tracking of account changes. THis is particluarly
    // useful for the SpecialUseInboxFolder set where we have decendent
    // Inbox account folders. We want to make sure we add/remove/update on all changes
    void trackAccountChanges();
    void accountsAdded(const QMailAccountIdList &idList);
    void accountsRemove(const QMailAccountIdList &idList);
    void accountsChanged(const QMailAccountIdList &idList);

private:
    FolderType m_type;
    QMailAccountIdList m_inboxList;
    void appendInboxDescendents();
    QMailMessageKey createAccountDescendentKey(const QMailAccountId &id, const QMailFolder::StandardFolder &folderType);
    QMailAccountIdList queryEnabledAccounts();
};

class SmartFolderSet : public MessageSet
{
    Q_OBJECT
    Q_PROPERTY(SmartFolderType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int smartCount READ smartCount NOTIFY smartCountChanged)

    Q_ENUMS(SmartFolderType)

public:
    explicit SmartFolderSet(QObject *parent =0);

    enum SmartFolderType {
        SmartTodoFolder,
        SmartDoneFolder,
        SmartTodayFolder,
        SmartCustomFolder
    };

    virtual void init(const QString &displayName, const QMailMessageKey &messageKey);
    virtual QVariant descendentsKey() { return QVariant(); } // We don't do descendents yet
    SmartFolderType type() const;
    int smartCount();
public slots:
    void setType(SmartFolderType type);
signals:
    void typeChanged(SmartFolderType type);
    void smartCountChanged();

private slots:
    void updateDescription();

private:
    SmartFolderType m_type;
    QTimer *m_timer; // Used to update SmartTodayFolder description
};

#endif // MESSAGESET_H
