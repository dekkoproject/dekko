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
#ifndef MESSAGEFILTERCOLLECTION_H
#define MESSAGEFILTERCOLLECTION_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "MessageSet.h"

// This component create a list of message sets based on some predefined criteria
class MessageFilterCollection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *children READ children NOTIFY childrenChanged)
    Q_PROPERTY(Filter filter READ filter WRITE setFilter NOTIFY filterChanged)

    Q_ENUMS(Filter)
public:
    explicit MessageFilterCollection(QObject *parent = 0);

    enum Filter {
        None,
        // Creates a list of merged standard folders
        // The StandardFolderSet has a childAccounts list model that can be used to select
        // a specific account inbox.
        StandardFolders,
        // Creates a list of smart folders.
        // Defaults are Todo, Done, Today and Custom smart folders
        SmartFolders
    };

    QObject *children() { return m_children; }

    Filter filter() const { return m_filter; }

signals:
    void filterChanged(Filter filter);
    void childrenChanged();

public slots:

    void setFilter(Filter filter);
    void reset();

protected:
    void createStandardFolderCollection();
    void createSmartFolderCollection();

private:
    QQmlObjectListModel<MessageSet> *m_children;
    Filter m_filter;
    bool m_firstRun;
};

#endif // MESSAGEFILTERCOLLECTION_H
