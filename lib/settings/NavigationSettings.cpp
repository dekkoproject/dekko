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
#include "NavigationSettings.h"
#include <QPointer>
#include <Paths.h>
#include "SettingsFileBase.h"

const QString NavigationKeys::defaultsCreated = QStringLiteral("defaults_created");
const QString NavigationKeys::unifiedInboxExpanded = QStringLiteral("unifiedinbox.expanded");
const QString NavigationKeys::favourites = QStringLiteral("favourites");
const QString NavigationKeys::favouriteExpanded = QStringLiteral("favourites.expanded");
const QString NavigationKeys::showFavourites = QStringLiteral("favourites.show");
const QString NavigationKeys::smarFolders = QStringLiteral("smartfolders");
const QString NavigationKeys::showSmartFolders = QStringLiteral("smartfolders.show");
const QString NavigationKeys::smartFoldersExpanded = QStringLiteral("smartfolders.expanded");
const QString NavigationKeys::accounts = QStringLiteral("accounts");
const QString NavigationKeys::accountsExpanded = QStringLiteral("accounts.expanded");
const QString NavigationKeys::showAccounts = QStringLiteral("accounts.show");

static QPointer<SettingsFileBase> navSettingsFile;

NavigationSettings::NavigationSettings(QObject *parent): SettingsObjectBase(parent)
{ 
    if (navSettingsFile.isNull()) {
        QString error;
        QScopedPointer<SettingsFileBase> file(new SettingsFileBase);
        QLockFile *lock = 0;
        QString navSettingsLock = Paths::configLocationForFile("navigation.json.lock");
        if (Paths::checkForStaleLockFile(&lock, navSettingsLock, error)) {
            qCritical() << error;
            qApp->quit();
        }
        navSettingsFile = new SettingsFileBase();
        navSettingsFile->setPath(Paths::configLocationForFile("navigation.json"));
        m_lock = new QScopedPointer<QLockFile>(lock);
    }
    setFilePath(navSettingsFile.data());
    if (m_file) {
        setPath(QStringLiteral("default"));
        createDefaultsIfNotExist();
    }
}

QString NavigationSettings::keyValue(NavigationSettings::Keys key)
{
    switch(key) {
    case DefaultsCreated:
        return NavigationKeys::defaultsCreated;
    case UnifiedInboxExpanded:
        return NavigationKeys::unifiedInboxExpanded;
    case Favourites:
        return NavigationKeys::favourites;
    case FavouritesExpanded:
        return NavigationKeys::favouriteExpanded;
    case FavouritesVisible:
        return NavigationKeys::showFavourites;
    case SmartFolders:
        return NavigationKeys::smarFolders;
    case SmartFoldersVisible:
        return NavigationKeys::showSmartFolders;
    case SmartFoldersExpanded:
        return NavigationKeys::smartFoldersExpanded;
    case Accounts:
        return NavigationKeys::accounts;
    case AccountsVisible:
        return NavigationKeys::showAccounts;
    case AccountsExpanded:
        return NavigationKeys::accountsExpanded;
    }
    Q_UNREACHABLE();
}

QVariant NavigationSettings::get(NavigationSettings::Keys key)
{
    return read(keyValue(key)).toVariant();
}

void NavigationSettings::set(Keys key, const QJsonValue &value)
{
    write(keyValue(key), value);
}

void NavigationSettings::createDefaultsIfNotExist()
{
    if (read(keyValue(DefaultsCreated)).toBool()) {
        return;
    }
    write(keyValue(UnifiedInboxExpanded), false);
    write(keyValue(FavouritesVisible), false);
    write(keyValue(FavouritesExpanded), false);
    write(keyValue(SmartFoldersVisible), true);
    write(keyValue(SmartFoldersExpanded), true);
    write(keyValue(AccountsVisible), true);
    write(keyValue(AccountsExpanded), true);
    write(keyValue(DefaultsCreated), true);
}

