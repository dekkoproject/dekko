#include <QCoreApplication>
#include <QDBusConnection>
#include <MailServiceWorker.h>
#include <MailServiceAdaptor.h>
#include <qmailnamespace.h>


#define SERVICE "org.dekkoproject.MailService"
#define SERVICE_PATH "/mail"


//Q_DECLARE_METATYPE(QList<quint64>)

Q_DECL_EXPORT int main(int argc, char** argv)
{

    QCoreApplication app(argc, argv);

    if(QMail::fileLock("dekko-worker.lock") == -1)
        qFatal("Could not get dekko worker lock. dekko-worker might already be running!");

    MailServiceWorker::registerTypes();

    MailServiceWorker *worker = Q_NULLPTR;
    MailServiceAdaptor *adaptor = Q_NULLPTR;

    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.interface()->isServiceRegistered(SERVICE)) {
        worker = new MailServiceWorker(&app);
        adaptor = new MailServiceAdaptor(worker);

        if (!connection.registerService(SERVICE)) {
            qFatal("Could not register service");
        }

        if (!connection.registerObject(SERVICE_PATH, worker)) {
            qFatal("Could not register MailServiceWorker object");
        }

    }

    return app.exec();
}
