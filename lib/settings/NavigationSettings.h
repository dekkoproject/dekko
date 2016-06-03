/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef NAVIGATIONSETTINGS_H
#define NAVIGATIONSETTINGS_H

#include <QObject>
#include <QVariantMap>
#include <QLockFile>
#include <QScopedPointer>
#include "SettingsObjectBase.h"

struct NavigationKeys {
    static const QString defaultsCreated, unifiedInboxExpanded;
    static const QString favourites, favouriteExpanded, showFavourites;
    static const QString smarFolders, showSmartFolders, smartFoldersExpanded;
    static const QString accounts, showAccounts, accountsExpanded;
};

class NavigationSettings : public SettingsObjectBase
{
    Q_OBJECT
    Q_ENUMS(Keys)
public:
    explicit NavigationSettings(QObject *parent = 0);

    enum Keys {
        DefaultsCreated = 0,
        UnifiedInboxExpanded,
        Favourites,
        FavouritesExpanded,
        FavouritesVisible,
        SmartFolders,
        SmartFoldersVisible,
        SmartFoldersExpanded,
        Accounts,
        AccountsVisible,
        AccountsExpanded
    };

    QString keyValue(Keys key);
    Q_INVOKABLE QVariant get(Keys key);
    Q_INVOKABLE void set(Keys key, const QJsonValue &value);

private:
    void createDefaultsIfNotExist();

    QScopedPointer<QLockFile> *m_lock;

};

#endif // NAVIGATIONSETTINGS_H
