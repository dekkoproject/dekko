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
#ifndef MSGPARTREPLY_H
#define MSGPARTREPLY_H

#include <QObject>
#include <QBuffer>
#include <QNetworkReply>
#include <qmailstore.h>
#include <qmailmessage.h>
#include <MailServiceClient.h>
#include "MsgPartQNAM.h"

struct PartFinder {
    QString location;
    const QMailMessagePart *part;
    PartFinder() {}
    PartFinder(const QString &location) : location(location) {}

    bool operator()(const QMailMessagePart &p) {
        const bool isPart = (p.location().toString(true) == location);
        if (isPart) {
            part = const_cast<QMailMessagePart *>(&p);
        }
        return !isPart; // Has to be false to exit the foreachPart
    }
};

struct CidPartDetector {
    QString cid;
    QString location;

    CidPartDetector(const QString &cid) : cid(cid) {}

    bool operator()(const QMailMessagePart &part) {
        const bool isPart = (part.contentID() == QUrl(cid).path());
        if (isPart) {
            location = part.location().toString(true);
        }
        return !isPart; // Has to be false to exit the foreachPart
    }
};

class MsgPartReply : public QNetworkReply
{
    Q_OBJECT
public:
    MsgPartReply(MsgPartQNAM *parent, const QMailMessageId &id, const QString &cidUrl);
    MsgPartReply(MsgPartQNAM *parent, const QMailMessageId &id, const QString &location,
                 const bool plaintext, const bool requiresFormatting);
    ~MsgPartReply();

    // QIODevice interface
public:
    virtual void close();
    virtual qint64 bytesAvailable() const;

protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    void failed(const QString &reason);
    const QMailMessagePart *partPtr( const bool refresh = false);

    // QNetworkReply interface
public slots:
    virtual void abort() { close(); }

    void handleMsgPartAvailable(const quint64 &id, const QString &location);
    void handlePartFetchFailed(const quint64 &id, const QString &location);

    void messageReady();
    void init();
protected:
    bool cidToPart(const QMailMessage &message, const QString &cidUrl);
private:
    // Container to hold our msgpart pointer.
    class Part {
    public:
        Part(const QMailMessageId &id, const QString &location) : m_finder(0), m_msg(id) {
            m_finder = PartFinder(location);
            m_msg.foreachPart<PartFinder &>(m_finder);
        }
        const QMailMessage parentMessage() {
            return m_msg;
        }

        const QMailMessagePart *partPtr() {
            return m_finder.part;
        }
    private:
        PartFinder m_finder;
        QMailMessage m_msg;
    };

    QByteArray *formattedBufferContent;
    QByteArray *secondBuffer;
    mutable QBuffer buffer;
    QMailMessageId m_msgId;
    QString m_location;
    bool m_ptext;
    bool m_format;
    Part *m_part;
};

#endif // MSGPARTREPLY_H
