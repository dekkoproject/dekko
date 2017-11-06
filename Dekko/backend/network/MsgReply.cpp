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
#include "MsgReply.h"
#include <QTimer>
#include <Formatting.h>

MsgReply::MsgReply(MsgPartQNAM *parent, QMailMessageId &msgId):
    QNetworkReply(parent), formattedBufferContent(0), m_id(msgId), m_msg(msgId), m_format(false)
{
    init();
    buffer.setBuffer(formattedBufferContent);
    buffer.open(QIODevice::ReadOnly);
}

MsgReply::MsgReply(MsgPartQNAM *parent, QMailMessageId &msgId, const bool requiresFormatting) :
    QNetworkReply(parent), formattedBufferContent(0), m_id(msgId), m_msg(msgId), m_format(requiresFormatting)
{
    init();
    buffer.setBuffer(formattedBufferContent);
    buffer.open(QBuffer::ReadOnly);
}

MsgReply::~MsgReply()
{
    qDebug() << "Deleteing MsgReply";
}

void MsgReply::close()
{
    if (formattedBufferContent) {
        delete formattedBufferContent;
        formattedBufferContent = 0;
    }
    buffer.close();
    buffer.setBuffer(0);
}

qint64 MsgReply::bytesAvailable() const
{
    return buffer.bytesAvailable() + QNetworkReply::bytesAvailable();
}

qint64 MsgReply::readData(char *data, qint64 maxlen)
{
    return buffer.read(data, maxlen);
}

void MsgReply::failed(const QString &reason)
{
    setError(TimeoutError, reason);
    setFinished(true);
    emit error(TimeoutError);
    emit finished();
    return;
}

void MsgReply::handleMsgsAvailable(const QMailMessageIdList &list)
{
    if (!list.contains(m_id)) {
        return;
    }
    messageReady();
}

void MsgReply::handleFetchFailed(const QMailMessageIdList &list)
{
    if (!list.contains(m_id)) {
        return;
    }
    failed(QStringLiteral("Failed downloading message"));
}

void MsgReply::messageReady()
{
    // Woohoo
    emit downloadProgress(2000, 2000);
    QByteArray data = m_msg.body().data().toUtf8();
    QMailMessageContentType ct = m_msg.body().contentType();
    if (m_format) {
        //do formatting
        data = Formatting::markupPlainTextToHtml(m_msg.body().data()).toUtf8();
        setHeader(QNetworkRequest::ContentTypeHeader, QString("text/html")); // Can't add charset :-(
    } else {
//        if (!ct.charset().isEmpty()) {
//            setHeader(QNetworkRequest::ContentTypeHeader, QString("%1; charset=%2").arg(QString(ct.content()), QString(ct.charset())));
//        } else {
//            setHeader(QNetworkRequest::ContentTypeHeader, QString("%1").arg(QString(ct.content())));
//        }
        setHeader(QNetworkRequest::ContentTypeHeader, ct.content());
    }
    formattedBufferContent = new QByteArray(data.data());
    setFinished(true);
    QTimer::singleShot( 0, this, SIGNAL(readyRead()) );
    QTimer::singleShot( 0, this, SIGNAL(finished()) );
}

void MsgReply::init()
{
    QUrl url;
    url.setScheme(QStringLiteral("dekko-msg"));
    url.setHost("msg");
    setUrl(url);

    setOpenMode(QIODevice::ReadOnly | QIODevice::Unbuffered);

    if (!m_msg.body().data().isEmpty()) {
        qDebug() << "[MsgReply]" << "Part available";
        messageReady();
    } else {
        qDebug() << "[MsgReply]" << "Part not available requesting now";
        connect(Client::instance(), SIGNAL(messagesNowAvailable(QMailMessageIdList)),
                this, SLOT(handleMsgsAvailable(QMailMessageIdList)));
        connect(Client::instance(), SIGNAL(messageFetchFailed(QMailMessageIdList)),
                this, SLOT(handleFetchFailed(QMailMessageIdList)));
        // Request the download now
        // If it's already fetched then "Client" will emit the messagePartNowAvailable immediately
        Client::instance()->downloadMessage(m_id);
        emit downloadProgress(1000, 2000);
    }

}



