#include "AccountsPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "Accounts.h"
#include "AccountConfiguration.h"
#include "PresetProviderModel.h"

void AccountsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Accounts"));
    // @uri Dekko.Accounts
    qmlRegisterType<Account>(uri, 1, 0, "Account");
    qmlRegisterType<NewAccount>(uri, 1, 0, "NewAccount");
    qmlRegisterType<AccountConfiguration>(uri, 1, 0, "AccountConfig");
    qmlRegisterType<ImapAccountConfiguration>(uri, 1, 0, "ImapAccountConfig");
    qmlRegisterType<SmtpAccountConfiguration>(uri, 1, 0, "SmtpAccountConfig");
    qmlRegisterType<PopAccountConfiguration>(uri, 1, 0, "PopAccountConfig");
    qmlRegisterType<Accounts>(uri, 1, 0, "Accounts");
    qmlRegisterType<PresetProviderModel>(uri, 1, 0, "PresetProviderModel");
    qmlRegisterUncreatableType<Provider>(uri, 1, 0, "Provider", "Cannot be created in QML. Instances created via PresetProviderModel");
}

void AccountsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
