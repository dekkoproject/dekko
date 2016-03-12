#ifndef FOLDER_H
#define FOLDER_H

#include <QObject>
#include <qmailfolder.h>
#include <qmailmessagekey.h>

class Folder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int folderId READ folderId WRITE setFolderId NOTIFY folderChanged)
    Q_PROPERTY(QVariant messageKey READ messageKey NOTIFY folderChanged)
    Q_PROPERTY(FolderType type READ type NOTIFY folderChanged)
    Q_PROPERTY(QString name READ name NOTIFY folderChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY countChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY countChanged)
    Q_PROPERTY(bool synchronizationEnabled READ syncEnabled NOTIFY folderChanged)
    Q_PROPERTY(bool isSynchronized READ synced NOTIFY folderChanged)
    Q_PROPERTY(bool partialContent READ partialContent NOTIFY folderChanged)
    Q_PROPERTY(bool canCreateChildren READ canCreateChildren NOTIFY folderChanged)
    Q_PROPERTY(bool canRename READ canRename NOTIFY folderChanged)
    Q_PROPERTY(bool canDelete READ canDelete NOTIFY folderChanged)
    Q_PROPERTY(bool isReadOnly READ readOnly NOTIFY folderChanged)
    Q_PROPERTY(bool isFavourite READ isFavourite NOTIFY folderChanged)
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
    QString name() const;
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

signals:
    void folderChanged();
    void countChanged();

public slots:
    void setFolderId(const int &id);
    void handleContentsModified(const QMailFolderIdList &idList);

private:
    QMailAccountId m_account;
    QMailFolder m_folder;
    QMailMessageKey m_key;
    FolderType m_type;

    int getUnreadCount();
};
#endif // FOLDER_H
