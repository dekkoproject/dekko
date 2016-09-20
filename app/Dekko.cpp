/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

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
#include "Dekko.h"
#include <QDebug>
#include <qmailnamespace.h>
#include <qmaillog.h>
#include <QUrl>
#include <QDirIterator>
#include <QtQuick/QQuickView>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickView>
#include <QQmlEngine>
#include <QCommandLineOption>
#include <QDir>
#include <QStandardPaths>

#define SMALL_FF_WIDTH 350
#define MEDIUM_FF_WDTH 800
#define LARGE_FF_WIDTH 1100

Dekko::Dekko(int &argc, char **argv) :
    QApplication(argc, argv),
#ifdef SERVER_AS_QTHREAD
    m_serverThread(0),
#else
    m_server(0),
#endif
    m_view(0),
    devMode(false),
    m_verboseLogging(false)
{
#ifdef CLICK_MODE
    QCoreApplication::setOrganizationName(QStringLiteral("dekko.dekkoproject"));
    QCoreApplication::setApplicationName(QStringLiteral("dekko.dekkoproject"));
 #else
    QCoreApplication::setOrganizationName(QStringLiteral("dekkoproject"));
    QCoreApplication::setApplicationName(QStringLiteral("dekko"));
#endif
    QCoreApplication::setApplicationVersion(QStringLiteral(DEKKO_VERSION));

    // Uncomment to dump out the resource files
    // Useful to be able to check a resource has been included
//    QDirIterator it(":", QDirIterator::Subdirectories);
//    while (it.hasNext()) {
//        qDebug() << it.next();
//    }
    parser.setApplicationDescription("Dekko email client");
    parser.addHelpOption();
    parser.addOption({"d", "Enable dev mode"});
    parser.addOption({"v", "Verbose logging"});
    parser.addOption({"small", "Open in small form factor state"});
    parser.addOption({"medium", "Open in medium form factor state"});
    parser.addOption({"large", "Open in large form factor state"});
    parser.addOption({"contacts", "Open the addressbook"});
    parser.process(*this);
}

bool Dekko::setup()
{
    QStringList arguments = this->arguments();
    Q_UNUSED(arguments);

    if (!QMail::mkLockDir()) {
        return false;
    }
#ifdef IS_UNITY8
    if (!QMail::mkLockDir()) {
        return false;
    }
    QDir appPath(QCoreApplication::applicationDirPath());
    addLibraryPath(appPath.absolutePath());
    QDir plugins5(appPath);
    if (plugins5.cd(QStringLiteral("../qmf/plugins5"))) {
        qDebug() << "Putting QMF_PLUGINS ENV";
        qputenv("QMF_PLUGINS", plugins5.absolutePath().toUtf8());
    } else {
        qDebug() << "QMF PLUGINS NOT SET";
    }

    if (appPath.cd(QStringLiteral("../Dekko/plugins"))) {
        qDebug() << "Putting DEKKO_PLUGINS ENV";
        qputenv("DEKKO_PLUGINS", appPath.absolutePath().toUtf8());
    } else {
        qDebug() << "DEKKO PLUGINS NOT SET";
    }
    qDebug() << "DEKKO_PLUGINS:" << qgetenv("DEKKO_PLUGINS");
    qDebug() << "QMF_PLUGINS: " << qgetenv("QMF_PLUGINS");
#endif
    qputenv("QMF_DATA", QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toUtf8());
    if (!isServerRunning()) {
        qDebug() << "[Dekko]" << "Message server not running attempting to start";
        if (!startServer()) {
            qDebug() << "[Dekko]" << "Message server failed to start";
            return false;
        } else {
            qDebug() << "[Dekko]" << "Message server started successfully \\o/";
        }
    } else {
        qDebug() << "[Dekko]" << "Message server already running, using that";
    }
    m_view = new QQuickView();
    m_view->engine()->setNetworkAccessManagerFactory(&m_partqnam);
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->setMinimumHeight(300);
    m_view->setMinimumWidth(300);
    if (parser.isSet(QStringLiteral("small"))) {
        m_view->setWidth(SMALL_FF_WIDTH);
    } else if (parser.isSet(QStringLiteral("medium"))) {
        m_view->setWidth(MEDIUM_FF_WDTH);
    } else {
        m_view->setWidth(LARGE_FF_WIDTH);
    }
    m_view->setHeight(550);
    m_view->setTitle("Dekko");

    devMode = parser.isSet("d");

    m_view->engine()->rootContext()->setContextProperty("dekkoapp", this);
    m_view->engine()->rootContext()->setContextProperty("ctxt_window", m_view);
    m_view->engine()->rootContext()->setContextProperty("devModeEnabled", QVariant(devMode));
    m_verboseLogging = (parser.isSet("d") || parser.isSet("v") || QFile::exists(QStringLiteral("/tmp/dekko-debug")));
    m_view->engine()->rootContext()->setContextProperty("verboseLogging", QVariant(m_verboseLogging));
    // Context property to figure out if we are on unity8/mir or not
    m_view->engine()->rootContext()->setContextProperty(QStringLiteral("isRunningOnMir"), QVariant(qgetenv("QT_QPA_PLATFORM") == "ubuntumirclient"));
    // Check both APP_URIS and positionalArgs for uris we need to handle
    QStringList uris;
    uris << qgetenv("APP_URIS");
    uris << parser.positionalArguments();
    if (parser.isSet("contacts")) {
        uris << QStringLiteral("dekko://contacts/open");
    }
    uris.removeDuplicates();
    m_view->engine()->rootContext()->setContextProperty(QStringLiteral("appUris"), uris);

    m_view->setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    m_view->show();
    return true;
}

bool Dekko::isServerRunning()
{
    int lockid = QMail::fileLock(QStringLiteral("messageserver-instance.lock"));
    if (lockid == -1)
        return true;

    QMail::fileUnlock(lockid);
    return false;
}

bool Dekko::startServer()
{
#ifdef SERVER_AS_QTHREAD
    // Use MessageServerThread
    m_serverThread = new MessageServerThread();
    m_serverThread->start();
    QEventLoop loop;
    QObject::connect(m_serverThread, &MessageServerThread::messageServerStarted, &loop, &QEventLoop::quit);
    loop.exec();
    return true;
#else
    if (m_server) {
        delete m_server;
        m_server = 0;
    }
    m_server = new QProcess(this);
    static const QString binary(QString("/messageserver5"));
    connect(m_server,SIGNAL(error(QProcess::ProcessError)),
            this,SLOT(serverProcessError(QProcess::ProcessError)));
    m_server->start(QMail::messageServerPath() + binary);
    return m_server->waitForStarted();
#endif
}

void Dekko::trimCache()
{
    m_view->engine()->trimComponentCache();
}
// TODO: show popup in mainview about server vanishing and Dekko will now close.
void Dekko::serverProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)
}
