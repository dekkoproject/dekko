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
#ifndef MSGREPLY_H
#define MSGREPLY_H

#include <QObject>
#include <QBuffer>
#include <QNetworkReply>
#include <qmailstore.h>
#include <MailServiceClient.h>
#include "MsgPartQNAM.h"

class MsgReply : public QNetworkReply
{
    Q_OBJECT
public:
    MsgReply(MsgPartQNAM *parent, QMailMessageId &msgId);
    MsgReply(MsgPartQNAM *parent, QMailMessageId &msgId, const bool requiresFormatting);
    ~MsgReply();

    // QIODevice interface
public:
    virtual void close();
    virtual qint64 bytesAvailable() const;

protected:
    virtual qint64 readData(char *data, qint64 maxlen);

    // QNetworkReply interface
public slots:
    virtual void abort() { close(); }
    void failed(const QString &reason);

    void handleMsgsAvailable(const QMailMessageIdList &list);
    void handleFetchFailed(const QMailMessageIdList &list);

    void messageReady();
    void init();

private:
    QByteArray *formattedBufferContent;
    mutable QBuffer buffer;
    QMailMessageId m_id;
    QMailMessage m_msg;
    bool m_format;
};

#endif // MSGREPLY_H
