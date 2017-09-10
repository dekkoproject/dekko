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
#ifndef FORBIDDENREPLY_H
#define FORBIDDENREPLY_H

#include <QNetworkReply>

/** @short A network reply which indicates that the request got denied by policy */
class ForbiddenReply : public QNetworkReply
{
    Q_OBJECT
public:
    explicit ForbiddenReply(QObject *parent);
    ForbiddenReply(QObject *parent, const QString &reason);
protected:
    virtual qint64 readData(char *data, qint64 maxSize) {
        Q_UNUSED(data); Q_UNUSED(maxSize);
        return -1;
    }
    virtual void abort() {}
private slots:
    void slotFinish();
};

#endif // FORBIDDENREPLY_H
