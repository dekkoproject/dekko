/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
#include <QtQml>
#include <QtQml/QQmlContext>
#include <libintl.h>
#include "emailplugin.h"
#include "MailAutoDiscover/autodiscover.h"
#include "MailAutoDiscover/emailvalidator.h"
#include "MailAutoDiscover/serverconfiguration.h"
#include "MailServiceValidator/servicevalidator.h"
#include "MailServiceValidator/mailservicetask.h"
#include "MailServiceValidator/saslmechanisms.h"

void EmailPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("accounts.plugin.Email"));
    // @uri accounts.plugin.Email
    qmlRegisterType<AutoDiscover>(uri, 1, 0, "AutoDiscover");
    qmlRegisterUncreatableType<ServerConfiguration>(uri, 1, 0, "ServerConfig", QLatin1String("ServerConfiguration cannot be instantiated from qml"));
    // export this as a singleton just incase it is handy to use for validation
    // in the qml layer
    qmlRegisterSingletonType<EmailValidator>(uri, 1, 0, "EmailValidator", EmailValidator::factory);

    qmlRegisterType<ServiceValidator>(uri, 1, 0, "ServiceValidator");
    qmlRegisterSingletonType<SaslMechanisms>(uri, 1, 0, "SaslMechanisms", SaslMechanisms::factory);
    qRegisterMetaType<MailServiceTask::State>();

}

void EmailPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    initTranslations();
}

void EmailPlugin::initTranslations()
{
    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, GETTEXT_LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
}
