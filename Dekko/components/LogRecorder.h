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
#ifndef LOGRECORDER_H
#define LOGRECORDER_H

#include <QObject>
#include <QmlObjectListModel.h>
//#include <QQmlAutoPropertyHelpers.h>
#include <QtQuick>
#include <QQmlEngine>
#include <QJSEngine>

class LogMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    QString m_message;

public:
    explicit LogMessage(QObject *parent = 0) : QObject(parent){}
    QString message() const
    {
        return m_message;
    }
public slots:
    void setMessage(QString message)
    {
        if (m_message == message)
            return;

        m_message = message;
        emit messageChanged(message);
    }
signals:
    void messageChanged(QString message);
};

class LogRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
    Q_ENUMS(Type)
public:
    explicit LogRecorder(QObject *parent = 0);
    enum Type {
        INFO,
        STATUS,
        WARNING,
        ERROR
    };
    QObject *model() { return m_model; }
    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

signals:
    void modelChanged();

public slots:
    void logMessage(const QString &location, const int &type, const QString &message);

private:
    QString formatMessage(const QString &location, const int &type, const QString &message);
    QQmlObjectListModel<LogMessage> *m_model;
};

#endif // LOGRECORDER_H
