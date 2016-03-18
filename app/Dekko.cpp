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

Dekko::Dekko(int &argc, char **argv) : QGuiApplication(argc, argv), m_server(0), m_view(0)
{
    setOrganizationName(QStringLiteral("dekkoproject"));
    setApplicationName(QStringLiteral("dekko"));
    // Uncomment to dump out the resource files
    // Useful to be able to check a resource has been included
//    QDirIterator it(":", QDirIterator::Subdirectories);
//    while (it.hasNext()) {
//        qDebug() << it.next();
//    }
}

bool Dekko::setup()
{
    QStringList arguments = this->arguments();
    Q_UNUSED(arguments);
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
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->setMinimumHeight(300);
    m_view->setMinimumWidth(300);
    m_view->setWidth(350);
    m_view->setHeight(550);
    m_view->setTitle("Dekko");
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

