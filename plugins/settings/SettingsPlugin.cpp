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
#include "SettingsPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <NavigationSettings.h>
#include <GlobalSettings.h>
#include <SettingsPolicies.h>
#include <PolicyManager.h>

void SettingsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Settings"));
    // @uri Dekko.Settings
    qmlRegisterType<NavigationSettings>(uri, 1, 0, "NavigationSettings");
    qmlRegisterType<GlobalSettings>(uri, 1, 0, "GlobalSettings");
    qmlRegisterSingletonType<PolicyManager>(uri, 1, 0, "PolicyManager", PolicyManager::factory);
    qmlRegisterType<MailPolicy>(uri, 1, 0, "MailPolicy");
    qmlRegisterUncreatableType<BasePolicy>(uri, 1, 0, "BasePolicy", "Cannot be created in qml");
}

void SettingsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
