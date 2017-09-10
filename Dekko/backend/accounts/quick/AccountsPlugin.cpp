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
#include "AccountsPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <Accounts.h>
#include <AccountConfiguration.h>
#include <AccountValidator.h>
#include <Identities.h>
#include "PresetProviderModel.h"

class AccountConfiguration;
void AccountsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Mail.Accounts"));
    // @uri Dekko.Accounts
    qmlRegisterType<Account>(uri, 1, 0, "Account");
    qmlRegisterType<AccountValidator>(uri, 1, 0, "AccountValidator");
    qmlRegisterType<NewAccount>(uri, 1, 0, "NewAccount");
    qmlRegisterType<AccountConfiguration>(uri, 1, 0, "AccountConfig");
    qmlRegisterType<ImapAccountConfiguration>(uri, 1, 0, "ImapAccountConfig");
    qmlRegisterType<SmtpAccountConfiguration>(uri, 1, 0, "SmtpAccountConfig");
    qmlRegisterType<PopAccountConfiguration>(uri, 1, 0, "PopAccountConfig");
    qmlRegisterType<Accounts>(uri, 1, 0, "Accounts");
    qmlRegisterType<PresetProviderModel>(uri, 1, 0, "PresetProviderModel");
    qmlRegisterType<Identities>(uri, 1, 0, "Identities");
    qmlRegisterUncreatableType<Identity>(uri, 1, 0, "Identity", "Cannot be created in QML. Instances created via PresetProviderModel");
    qmlRegisterUncreatableType<Provider>(uri, 1, 0, "Provider", "Cannot be created in QML. Instances created via PresetProviderModel");
}

void AccountsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
