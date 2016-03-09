#include "AccountsPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "Account.h"

void AccountsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Accounts"));
    // @uri Dekko.Accounts
      qmlRegisterType<Account>(uri, 1, 0, "Account");
}

void AccountsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
