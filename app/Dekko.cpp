#include "Dekko.h"
#include <QDebug>
#include <qmailnamespace.h>
#include <QUrl>
#include <QDirIterator>
#include <QtQuick/QQuickView>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickView>
#include <QQmlEngine>
#include <QCommandLineOption>
#include <QStandardPaths>

#define SMALL_FF_WIDTH 350
#define MEDIUM_FF_WDTH 800
#define LARGE_FF_WIDTH 1100

Dekko::Dekko(int &argc, char **argv) :
    QGuiApplication(argc, argv), m_server(0), m_view(0), devMode(false), m_verboseLogging(false)
{
    if (CLICK_MODE) {
        setOrganizationName(QStringLiteral("dekko.dekkoproject"));
        setApplicationName(QStringLiteral("dekko.dekkoproject"));
    } else {
        setOrganizationName(QStringLiteral("dekkoproject"));
        setApplicationName(QStringLiteral("dekko"));
    }
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
    parser.process(*this);
}

bool Dekko::setup()
{
    QStringList arguments = this->arguments();
    Q_UNUSED(arguments);
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
    m_view->engine()->rootContext()->setContextProperty("ctxt_window", m_view);
    m_view->engine()->rootContext()->setContextProperty("devModeEnabled", QVariant(devMode));
    m_verboseLogging = (parser.isSet("v") || QFile::exists(QStringLiteral("/tmp/dekko-debug")));
    m_view->engine()->rootContext()->setContextProperty("verboseLogging", QVariant(m_verboseLogging));
    // Context property to figure out if we are on unity8/mir or not
    m_view->engine()->rootContext()->setContextProperty(QStringLiteral("isRunningOnMir"), QVariant(qgetenv("QT_QPA_PLATFORM") == "ubuntumirclient"));

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
}
// TODO: show popup in mainview about server vanishing and Dekko will now close.
void Dekko::serverProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)
}

