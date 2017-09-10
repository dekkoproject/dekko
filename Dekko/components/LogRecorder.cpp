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
#include "LogRecorder.h"

LogRecorder::LogRecorder(QObject *parent) : QObject(parent), m_model(0)
{
    m_model = new QQmlObjectListModel<LogMessage>(this);
    emit modelChanged();
}

QObject *LogRecorder::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    LogRecorder *lr = new LogRecorder(reinterpret_cast<QObject*>(engine));
    return lr;
}

void LogRecorder::logMessage(const QString &location, const int &type, const QString &message)
{
    if (m_model->count() > 250) {
        m_model->remove(m_model->first());
    }
    if (message.isEmpty()) {
        return;
    }
    LogMessage *msg = new LogMessage(0);
    msg->setMessage(formatMessage(location, type, message));
    m_model->append(msg);
}
// error #ee4035
// info #7bc043
// warning #9AD3EB
// status #ffd633
// text #00aba9
// text err #f37736
QString LogRecorder::formatMessage(const QString &location, const int &type, const QString &message)
{
    if (message.isEmpty()) {
        return QString();
    }

    QString text;
    switch ((Type)type) {
    case INFO:
        text = QStringLiteral("<font color=\"#f09609\">[%1]&nbsp;</font><font color=\"#7bc043\">[INFO]&nbsp;</font><font color=\"#00aba9\">%2</font>").arg(location, message);
        break;
    case STATUS:
        text = QStringLiteral("<font color=\"#f09609\">[%1]&nbsp;</font><font color=\"#ffd633\">[STATUS]&nbsp;</font><font color=\"#00aba9\">%2</font>").arg(location, message);
        break;
    case WARNING:
        text = QStringLiteral("<font color=\"#f09609\">[%1]&nbsp;</font><font color=\"#9AD3EB\">[WARNING]&nbsp;</font><font color=\"#00aba9\">%2</font>").arg(location, message);
        break;
    case ERROR:
        text = QStringLiteral("<font color=\"#f09609\">[%1]&nbsp;</font><font color=\"#ee4035\">[ERROR]&nbsp;</font><font color=\"#f37736\">%2</font>").arg(location, message);
        break;
    }

    if (text.isEmpty()) {
        text = message;
    }
    return text;
}

