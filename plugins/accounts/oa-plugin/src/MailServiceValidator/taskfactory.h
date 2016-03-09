/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include <QObject>
#include "mailservicetask.h"
#include "imaptask.h"
#include "smtptask.h"

enum class Task {
    IMAP,
    SMTP
};

class TaskFactory : public QObject
{
    Q_OBJECT
public:

    static MailServiceTask *createTask(QObject *p, QObject* account, const Task &task) {
        switch (task) {
        case Task::IMAP:
            return new ImapTask(p, account);
        case Task::SMTP:
            return new SmtpTask(p, account);
        }
    }
};


#endif // TASKFACTORY_H
