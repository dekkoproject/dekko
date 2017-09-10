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
#ifndef MSGPARTQNAM_H
#define MSGPARTQNAM_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QSharedPointer>
#include <qmailmessage.h>

struct Allowed {
    static const QStringList urlSchemes, hosts;
};

class MsgPartQNAM : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit MsgPartQNAM(QObject *parent = 0);
    virtual ~MsgPartQNAM();

signals:

public slots:

    // QNetworkAccessManager interface
protected:
    virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

private:
    QMailMessagePart m_part;
};

#endif // MSGPARTQNAM_H
