#ifndef MESSAGESET_H
#define MESSAGESET_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <qmailmessagekey.h>
#include <qmailfolder.h>

class MessageSet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hasDescendents READ hasDecendents NOTIFY descendentsCountChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QObject *descendents READ children CONSTANT)
    Q_PROPERTY(QVariant messageKey READ messageKey NOTIFY messageKeyChanged)
    Q_PROPERTY(QVariant descendentsKey READ descendentsKey NOTIFY descendentsCountChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY countChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY countChanged)

public:
    explicit MessageSet(QObject *parent = 0);
    virtual void init(const QString &displayName, const QMailMessageKey &messageKey) = 0;
    QObject *children() { return m_children; }
    QVariant messageKey() const { return m_key; }
    QVariant descendentsKey() const;
    QString displayName() const { return m_name; }
    bool hasDecendents() const { return !m_children->isEmpty(); }
    int unreadCount();
    int totalCount();

signals:
    void messageKeyChanged();
    void descendentsCountChanged();
    void displayNameChanged();
    void countChanged();

public slots:
    void setMessageKey(const QMailMessageKey &key);
    void setDisplayName(const QString &displayName);

protected:
    QString m_name;
    QQmlObjectListModel<MessageSet> *m_children;
    QMailMessageKey m_key;
};

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

#endif // MESSAGESET_H
