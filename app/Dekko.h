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
