#include <QGuiApplication>
#include <QtQuick>
#include <MazDB.h>
#include <MazDBBatch.h>
#include <MazOptions.h>
#include <MazDBListModel.h>
#include "MazDBSettings.h"
#include "MazDBSortProxy.h"
#include "MazDBQueryEngine.h"
#include "MazDBQuery.h"
#include "Mace.h"

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

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


    Mace::Runner runner;
    runner.include(QML_TESTS_PATH);
    return runner.run(app.arguments());
}
