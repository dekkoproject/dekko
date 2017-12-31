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
#ifndef DEKKO_H
#define DEKKO_H

#include <QObject>
#include <QLoggingCategory>
//#include <QGuiApplication>
#include <QApplication>
#include <QPointer>
#include <QProcess>
#include <QtQuick/QQuickView>
#include <QQmlApplicationEngine>
#include <MsgPartQNAMFactory.h>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <ServiceRegistry.h>

#ifdef SERVER_AS_QTHREAD
#include <messageserver.h>
#endif

Q_DECLARE_LOGGING_CATEGORY(DEKKO_MAIN)

class Dekko : public QApplication
{
    Q_OBJECT
public:
    Dekko(int &argc, char **argv);
    ~Dekko();
    /** @short should be run before call to exec() */
    bool setup();
    bool isServerRunning();
    bool startServer();

    bool isWorkerRunning();
    bool startWorker();

    Q_INVOKABLE void trimCache();

signals:

public slots:

private slots:
    void serverProcessError(QProcess::ProcessError error);
    void workerProcessError(QProcess::ProcessError error);

protected:
    void loadPlugins();

private:
    ServiceRegistry *m_serviceRegistry;
#ifdef SERVER_AS_QTHREAD
    MessageServerThread *m_serverThread;
#else
    QProcess *m_server;
    QProcess *m_worker;
#endif
    MsgPartQNAMFactory m_partqnam;
    bool devMode;
    bool m_verboseLogging;
    QCommandLineParser parser;
    QQmlApplicationEngine m_engine;

};

#endif // DEKKO_H
