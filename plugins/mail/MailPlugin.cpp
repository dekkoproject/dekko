#include "MailPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include "Client.h"
#include "FolderList.h"
#include "Folder.h"
#include "MessageFilterCollection.h"
#include "MessageSet.h"
#include "MessageList.h"
#include "MailboxSearch.h"
#include "qmlenums.h"
#include "SenderIdentities.h"

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
    qmlRegisterType<MailboxSearch>(uri, 1, 0, "MailboxSearch");
    qmlRegisterType<SenderIdentities>(uri, 1, 0, "SenderIdentities");
    qmlRegisterUncreatableType<MessageSet>(uri, 1, 0, "MessageSet", "Cannot create MessageSet from QML, c++ only chap!");
    qmlRegisterUncreatableType<StandardFolderSet>(uri, 1, 0, "StandardFolderSet", "Cannot create MessageSet from QML, c++ only chap!");
    qmlRegisterUncreatableType<SmartFolderSet>(uri, 1, 0, "SmartFolderSet", "Cannot create SmartFolderSet from QML, c++ only chap!");
    qmlRegisterUncreatableType<SearchService>(uri, 1, 0, "SearchService", "Cannot create SearchService from QML, c++ only chap!");
    qmlRegisterSingletonType<Client>(uri, 1, 0, "Client", Client::factory);
    qmlRegisterUncreatableType<RecipientType>(uri, 1, 0, "RecipientType", "Cannot create an instance in QML");
}

void MailPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

