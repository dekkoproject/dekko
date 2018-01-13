/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

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
#include <SnapStandardPaths.h>
#include <PluginRegistry.h>

#define SMALL_FF_WIDTH 350
#define MEDIUM_FF_WDTH 800
#define LARGE_FF_WIDTH 1100

Q_LOGGING_CATEGORY(DEKKO_MAIN, "dekko.main")

Dekko::Dekko(int &argc, char **argv) :
    QApplication(argc, argv),
    m_serviceRegistry(0),
#ifdef SERVER_AS_QTHREAD
    m_serverThread(0),
#else
    m_server(0),
    m_worker(0),
#endif
    devMode(false),
    m_verboseLogging(false)
{
    QCoreApplication::setOrganizationName(APP_ORG);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(DEKKO_VERSION);

    // Uncomment to dump out the resource files
    // Useful to be able to check a resource has been included
//    QDirIterator it(":", QDirIterator::Subdirectories);
//    while (it.hasNext()) {
//        qCDebug(DEKKO_MAIN) << it.next();
//    }
    parser.setApplicationDescription("Dekko email client");
    parser.addHelpOption();
    parser.addOption({"d", "Enable dev mode"});
    parser.addOption({"v", "Verbose logging"});
    parser.addOption({"small", "Open in small form factor state"});
    parser.addOption({"medium", "Open in medium form factor state"});
    parser.addOption({"large", "Open in large form factor state"});
    parser.addOption({"contacts", "Open the addressbook"});

    QStringList cArgs = this->arguments();

    for (const QString &s : cArgs) {
        if (s.startsWith("--desktop_file_hint=")) {
            cArgs.removeAll(s);
        }
    }

    parser.process(cArgs);

    m_verboseLogging = (parser.isSet("d") || parser.isSet("v") || QFile::exists(QStringLiteral("/tmp/dekko-debug")));

    if(!m_verboseLogging)
            qputenv("QT_LOGGING_RULES", "dekko.*=false");
        else
            qputenv("QT_LOGGING_RULES", "dekko.*=true");

}

Dekko::~Dekko(){
#ifndef SERVER_AS_QTHREAD
    delete m_server;
    m_server = 0;
    delete m_worker;
    m_worker = 0;
#endif
}

bool Dekko::setup()
{
    QStringList arguments = this->arguments();
    Q_UNUSED(arguments);

    if (!QMail::mkLockDir()) {
        return false;
    }

    if (qgetenv("QMF_DATA").isEmpty()) {
        // Fall back to standard xdg cache location
        qputenv("QMF_DATA", SnapStandardPaths::writableLocation(SnapStandardPaths::AppCacheLocation).toUtf8());
    }


    loadPlugins();
    m_serviceRegistry = new ServiceRegistry(this);
    m_serviceRegistry->setServiceKey(QStringLiteral("Dekko::Service"));

#if defined(CLICK_MODE)
    m_serviceRegistry->startServices();
#else
    if (!isServerRunning()) {
        qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message server not running attempting to start";
        if (!startServer()) {
            qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message server failed to start";
            return false;
        } else {
            qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message server started successfully \\o/";
        }
    } else {
        qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message server already running, using that";
    }
    m_serviceRegistry->startServices();
#endif

    if (!isWorkerRunning()) {
        qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message worker not running attempting to start";
        if (!startWorker()) {
            qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message worker failed to start";
            return false;
        } else {
            qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message worker started successfully \\o/";
        }
    } else {
        qCDebug(DEKKO_MAIN) << "[Dekko]" << "Message worker already running, using that";
    }
    m_engine.setNetworkAccessManagerFactory(&m_partqnam);

    devMode = parser.isSet("d");

    m_engine.rootContext()->setContextProperty("dekkoapp", this);
    m_engine.rootContext()->setContextProperty("service", m_serviceRegistry);
    m_engine.rootContext()->setContextProperty("devModeEnabled", QVariant(devMode));
    m_engine.rootContext()->setContextProperty("verboseLogging", QVariant(m_verboseLogging));
    // Context property to figure out if we are on unity8/mir or not
    m_engine.rootContext()->setContextProperty(QStringLiteral("isRunningOnMir"), QVariant(qgetenv("QT_QPA_PLATFORM") == "ubuntumirclient"));
    // Check both APP_URIS and positionalArgs for uris we need to handle
    QStringList uris;
    uris << qgetenv("APP_URIS");
    uris << parser.positionalArguments();
    if (parser.isSet("contacts")) {
        uris << QStringLiteral("dekko://contacts/open");
    }
    uris.removeDuplicates();
    m_engine.rootContext()->setContextProperty(QStringLiteral("appUris"), uris);
    m_engine.load(QUrl("qrc:/qml/Dekko.qml"));
    return true;
}

bool Dekko::isServerRunning()
{
    int lockid = QMail::fileLock(QStringLiteral("dekkod-instance.lock"));
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
    static const QString binary(QString("/dekkod"));
    connect(m_server,SIGNAL(error(QProcess::ProcessError)),
            this,SLOT(serverProcessError(QProcess::ProcessError)));
    m_server->start(QMail::messageServerPath() + binary);
    return m_server->waitForStarted();
#endif
}

bool Dekko::isWorkerRunning()
{
    int lockid = QMail::fileLock(QStringLiteral("dekko-worker.lock"));
    if (lockid == -1)
        return true;

    QMail::fileUnlock(lockid);
    return false;
}

bool Dekko::startWorker()
{
    if (m_worker) {
        delete m_worker;
        m_worker = 0;
    }
    m_worker = new QProcess(this);
    static const QString binary(QString("/dekko-worker"));
    connect(m_worker,SIGNAL(error(QProcess::ProcessError)),
            this,SLOT(workerProcessError(QProcess::ProcessError)));
    connect(m_worker, &QProcess::readyRead, [=](){ if (m_worker->canReadLine()) qCDebug(DEKKO_MAIN) << m_worker->readLine(); });
    m_worker->start(QMail::messageServerPath() + binary);
    return m_worker->waitForStarted();
}

void Dekko::trimCache()
{
    m_engine.trimComponentCache();
}
// TODO: show popup in mainview about server vanishing and Dekko will now close.
void Dekko::serverProcessError(QProcess::ProcessError error)
{
    qWarning() << "[ERROR] Message server stopped, trying to restart";
    delete m_server;
    m_server = 0;
    startServer();
}

void Dekko::workerProcessError(QProcess::ProcessError error)
{
    qWarning() << "[ERROR] Message worker stopped, trying to restart";
    delete m_worker;
    m_worker = 0;
    startWorker();
}

void Dekko::loadPlugins()
{
    PluginRegistry::instance()->setPluginLocations(
                QStringList()
                << QStringLiteral("Dekko::Mail")
                << QStringLiteral("Dekko::Mail::Composer")
                << QStringLiteral("Dekko::Mail::Composer::UI")
                << QStringLiteral("Dekko::Mail::Settings")
                << QStringLiteral("Dekko::Mail::SetupWizard")
                << QStringLiteral("Dekko::Stage::Settings")
                << QStringLiteral("Dekko::Settings::AccountAction")
                << QStringLiteral("Dekko::Settings::PluginAction")
                << QStringLiteral("Dekko::Settings::MailAction")
                << QStringLiteral("Dekko::Stage::Main")
                << QStringLiteral("Dekko::Stage::Mail")
                << QStringLiteral("Dekko::Stage::Contacts")
                << QStringLiteral("Dekko::Stage::Contacts::UI")
                << QStringLiteral("Dekko::Stage::Calendar")
                << QStringLiteral("Dekko::Stage::Calendar::UI")
                << QStringLiteral("Dekko::Stage::Notes")
                << QStringLiteral("Dekko::Stage::Notes::UI")
                << QStringLiteral("Dekko::Stage::Action")
                << QStringLiteral("Dekko::Mail::NavHeadAction")
                << QStringLiteral("Dekko::Service")
                << QStringLiteral("Dekko::Listener")
                << QStringLiteral("Dekko::AddressBook::Provider")
                << QStringLiteral("Dekko::Mail::MultiSelectAction")
                << QStringLiteral("Dekko::Mail::FolderPicker")
                );

   PluginRegistry::instance()->loadPlugins(
               QStringList()
               << QStringLiteral("%1/plugins").arg(SnapStandardPaths::writableLocation(SnapStandardPaths::AppDataLocation))
               );
}
