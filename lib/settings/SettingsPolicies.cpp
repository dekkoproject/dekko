/**************************************************************************
   Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
**************************************************************************/
#include "SettingsPolicies.h"

int MailPolicy::markInterval()
{
    if (idValid()) {
        return readPolicy(QStringLiteral("mail.markReadInterval")).toInt();
    }
    return 1000;
}

void MailPolicy::setMarkInterval(const int &interval)
{
    if (!idValid()) {
        qDebug() << "Invalid ID cannot set interval policy";
    }
    setPolicy(QStringLiteral("mail.markReadInterval"), QString::number(interval));
    emit policyChanged();
}

MailPolicy::MarkReadMode MailPolicy::markRead()
{
    QString policy = readPolicy(QStringLiteral("mail.markAsRead"));
    if (!policy.isEmpty()) {
        if (policy == QStringLiteral("never")) {
            return MarkReadMode::Never;
        } else if ( policy == QStringLiteral("interval")) {
            return MarkReadMode::AfterInterval;
        } else if (policy == QStringLiteral("immediate")) {
            return MarkReadMode::Immediately;
        }
    }
    return MarkReadMode::AfterInterval;
}

void MailPolicy::setMarkRead(const MailPolicy::MarkReadMode mode)
{
    switch(mode) {
    case MarkReadMode::Never:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("never"));
        break;
    case MarkReadMode::AfterInterval:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("interval"));
        break;
    case MarkReadMode::Immediately:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("immediate"));
        break;
    default:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("interval"));
        break;
    }
    emit policyChanged();
}

void MailPolicy::setDefaults()
{
    setMarkRead(MarkReadMode::AfterInterval);
    setMarkInterval(1000);
}
