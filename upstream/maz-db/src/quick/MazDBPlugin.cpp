#include "MazDBPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <MazDB.h>
#include <MazDBBatch.h>
#include <MazDBListModel.h>
#include <MazDBSettings.h>
#include <MazDBSortProxy.h>
#include <MazOptions.h>
#include <MazDBQueryEngine.h>
#include <MazDBQuery.h>

void MazDBPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("MazDB"));
    // @uri MazDB
    qmlRegisterType<MazDB>("MazDB", 1, 0, "MazDB");
    qmlRegisterType<MazDBListModel>("MazDB", 1, 0, "MazDBListModel");
    qmlRegisterType<MazDBSettings>("MazDB", 1, 0, "MazDBSettings");
    qmlRegisterType<MazDBSortProxy>("MazDB", 1, 0, "MazDBSortProxy");
    qmlRegisterType<MazDBQueryEngine>("MazDB", 1, 0, "MazDBQueryEngine");
    qmlRegisterType<MazDBQuery>("MazDB", 1, 0, "MazDBQuery");
    qmlRegisterType<WhereQuery>("MazDB", 1, 0, "WHERE");
    qmlRegisterType<AndQuery>("MazDB", 1, 0, "AND");
    qmlRegisterType<OrQuery>("MazDB", 1, 0, "OR");
    qmlRegisterUncreatableType<MazOptions>("MazDB", 1, 0, "MazOptions", QObject::tr("Cannot create separate instance of Options"));
    qmlRegisterUncreatableType<MazDBBatch>("MazDB", 1, 0, "MazDBBatch", QObject::tr("Cannot create separate instance of Batch"));
    qmlRegisterUncreatableType<QueryBase>("MazDB", 1, 0, "Query", QObject::tr("Cannot create separate instance of QueryBase"));
    qmlRegisterUncreatableType<Range>("MazDB", 1, 0, "Range", QObject::tr("Cannot create separate instance of Range"));
}

void MazDBPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
