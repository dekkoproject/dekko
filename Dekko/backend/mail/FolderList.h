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
