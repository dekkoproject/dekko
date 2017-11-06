/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "messageserver.h"
#include <QCoreApplication>
#include <qmailnamespace.h>
#include <qmaillog.h>
#include <qloggers.h>
#include <signal.h>
#ifdef USE_HTML_PARSER
#include <QtGui>
#endif

#if defined(CLICK)
#define APP_NAME "dekko2.dekkoproject"
#else
#define APP_NAME "dekko"
#endif

#if !defined(NO_SHUTDOWN_SIGNAL_HANDLING) && defined(Q_OS_UNIX)

static void shutdown(int n)
{
    qDebug() << "Received signal" << n << ", shutting down.";
#ifdef USE_HTML_PARSER
    QGuiApplication::exit();
#else
    QCoreApplication::exit();
#endif
}
#endif

#if defined(Q_OS_UNIX)

static void recreateLoggers(int n)
{
    qMailLoggersRecreate(APP_NAME, "dekkod", "Msgsrv");
    qDebug() << "Received signal" << n << ", logs recreated.";
}
#endif

Q_DECL_EXPORT int main(int argc, char** argv)
{
#ifdef USE_HTML_PARSER
    // Need for html parsing by <QTextdocument> in qmailmessage.cpp
    QGuiApplication app(argc, argv);
#else
    QCoreApplication app(argc, argv);
#endif

    // This is ~/.config/dekko.dekkoproject/dekkod.conf
    qMailLoggersRecreate(APP_NAME, "dekkod", "Msgsrv");

    if(QMail::fileLock("dekkod-instance.lock") == -1)
        qFatal("Could not get messageserver lock. Messageserver might already be running!");

    MessageServer server;

#if !defined(NO_SHUTDOWN_SIGNAL_HANDLING) && defined(Q_OS_UNIX)
    signal(SIGINT, shutdown);
    signal(SIGTERM, shutdown);
#endif

#if defined(Q_OS_UNIX)
    signal(SIGHUP,recreateLoggers);
#endif

    int exitCode = app.exec();

    return exitCode;
}

