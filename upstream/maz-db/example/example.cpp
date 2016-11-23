#include <QGuiApplication>
#include <QtQuick>
#include <QQmlApplicationEngine>
#include <MazDB.h>
#include <MazDBBatch.h>
#include <MazOptions.h>
#include <MazDBListModel.h>
#include <MazDBSettings.h>
#include <MazDBSortProxy.h>
#include <MazDBQueryEngine.h>
#include <MazDBQuery.h>
#include <QUrl>
#include <QDirIterator>
#include <QDebug>

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    return app.exec();
}
