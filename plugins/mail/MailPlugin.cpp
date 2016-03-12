#include "MailPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "FolderList.h"

void MailPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Mail"));
    // @uri Dekko.Mail
      qmlRegisterType<Folder>(uri, 1, 0, "Folder");
      qmlRegisterType<FolderList>(uri, 1, 0, "FolderList");
}

void MailPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

