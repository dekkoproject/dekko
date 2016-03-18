#include "AccountsPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "Accounts.h"
#include "AccountConfiguration.h"

void AccountsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Accounts"));
    // @uri Dekko.Accounts
    qmlRegisterType<Account>(uri, 1, 0, "Account");
    qmlRegisterType<AccountConfiguration>(uri, 1, 0, "AccountConfig");
    qmlRegisterType<ImapAccountConfiguration>(uri, 1, 0, "ImapAccountConfig");
    qmlRegisterType<SmtpAccountConfiguration>(uri, 1, 0, "SmtpAccountConfig");
    qmlRegisterType<PopAccountConfiguration>(uri, 1, 0, "PopAccountConfig");
    qmlRegisterType<Accounts>(uri, 1, 0, "Accounts");
}

void AccountsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
