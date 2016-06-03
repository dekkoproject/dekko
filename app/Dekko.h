#ifndef DEKKO_H
#define DEKKO_H

#include <QObject>
#include <QGuiApplication>
#include <QPointer>
#include <QProcess>
#include <QtQuick/QQuickView>
#include <QQmlApplicationEngine>
#include <MsgPartQNAMFactory.h>
#include <QCommandLineParser>

class Dekko : public QGuiApplication
{
    Q_OBJECT
public:
    Dekko(int &argc, char **argv);
    virtual ~Dekko(){}
    /** @short should be run before call to exec() */
    bool setup();
    bool isServerRunning();
    bool startServer();

signals:

public slots:

private slots:
    void serverProcessError(QProcess::ProcessError error);

private:
    QProcess *m_server;
    QQuickView *m_view;
    MsgPartQNAMFactory m_partqnam;
    bool devMode;
    bool m_verboseLogging;
    QCommandLineParser parser;
};

#endif // DEKKO_H
