#include "MailPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "FolderList.h"
#include "Folder.h"
#include "MessageFilterCollection.h"
#include "MessageSet.h"
#include "MessageList.h"

void MailPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Mail"));
    // @uri Dekko.Mail
    qmlRegisterType<Folder>(uri, 1, 0, "Folder");
    qmlRegisterType<FolderList>(uri, 1, 0, "FolderList");
    qmlRegisterType<MessageFilterCollection>(uri, 1, 0, "MessageFilterCollection");
    qmlRegisterType<MessageList>(uri, 1, 0, "MessageList");
    qmlRegisterType<MinimalMessage>(uri, 1, 0, "MinimalMessage");
    qmlRegisterType<Message>(uri, 1, 0, "Message");
    qmlRegisterUncreatableType<MessageSet>(uri, 1, 0, "MessageSet", "Cannot create MessageSet from QML, c++ only chap!");
    qmlRegisterUncreatableType<StandardFolderSet>(uri, 1, 0, "StandardFolderSet", "Cannot create MessageSet from QML, c++ only chap!");
}

void MailPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

