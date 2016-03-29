#ifndef FOLDERLIST_H
#define FOLDERLIST_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <qmailaccount.h>
#include "Folder.h"

class FolderList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
    Q_PROPERTY(quint64 accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)
public:
    explicit FolderList(QObject *parent = 0);

    QObject *model() { return m_model; }
    quint64 accountId() const { return m_accountId.toULongLong(); }


    static bool sortIdList(const QMailFolderId &a, const QMailFolderId &b);

signals:
    void modelChanged();
    void accountIdChanged();

public slots:
    void reset();
    void setAccountId(const quint64 &id);
    void addFolder(const QMailFolderId &folderId, QMailMessageKey key, QMailFolderIdList &folderList);
    void insertFolder(const QMailFolderId &id, const Folder::FolderType &type, const QMailMessageKey &key);

private:
    QQmlObjectListModel<Folder> *m_model;
    QMailAccountId m_accountId;
};

#endif // FOLDERLIST_H
