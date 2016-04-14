#include "MsgPartReply.h"
#include <Formatting.h>
#include <QTimer>

MsgPartReply::MsgPartReply(MsgPartQNAM *parent, const QMailMessageId &id, const QString &cidUrl):
    QNetworkReply(parent), formattedBufferContent(0), m_msgId(id), m_ptext(false), m_format(false),
    m_part(0)
{
    if (!cidToPart(QMailMessage(m_msgId), cidUrl)) {
        failed(QStringLiteral("Failed to find cid part"));
        return;
    }
    m_part = new Part(m_msgId, m_location);
    if (m_part->partPtr()->contentID().isEmpty()) {
        qDebug() << "MISSING CONTENT ID";
        failed(QStringLiteral("Missing content id"));
        return;
    }
    init();
    buffer.setBuffer(formattedBufferContent);
    buffer.open(QIODevice::ReadOnly);
}

MsgPartReply::MsgPartReply(MsgPartQNAM *parent, const QMailMessageId &id,
                           const QString &location, const bool plaintext,
                           const bool requiresFormatting) :
    QNetworkReply(parent), formattedBufferContent(0),
    m_msgId(id), m_location(location), m_ptext(plaintext), m_format(requiresFormatting),
    m_part(0)
{
    m_part = new Part(m_msgId, m_location);
    init();
    buffer.setBuffer(formattedBufferContent);
    buffer.open(QIODevice::ReadOnly);
}

void MsgPartReply::init()
{
    QUrl url;
    url.setScheme(QStringLiteral("dekko-part"));
    url.setHost("msg");
    setUrl(url);

    setOpenMode(QIODevice::ReadOnly | QIODevice::Unbuffered);
    if (auto part = m_part->partPtr()) {
        qDebug() << part->location().toString(true);
        if (part->contentAvailable()) {
            qDebug() << "[MsgPartReply]" << "Part available";
            messageReady();
        } else {
            qDebug() << "[MsgPartReply]" << "Part not available requesting now";
            connect(Client::instance(), SIGNAL(messagePartNowAvailable(quint64,QString)),
                    this, SLOT(handleMsgPartAvailable(quint64,QString)));
            connect(Client::instance(), SIGNAL(messagePartFetchFailed(quint64,QString)),
                    this, SLOT(handlePartFetchFailed(quint64,QString)));
            // Request the download now
            // If it's already fetched then "Client" will emit the messagePartNowAvailable immediately
            Client::instance()->downloadMessagePart(m_part->partPtr());
            // just fudge download progress for now
            emit downloadProgress(1000, 2000);
        }
    }
}

bool MsgPartReply::cidToPart(const QMailMessage &message, const QString &cidUrl) {

    CidPartDetector d(cidUrl);
    if (!message.foreachPart<CidPartDetector &>(d)) {
        m_location = d.location;
        return true;
    } else {
        return false;
    }
}

void MsgPartReply::close()
{
    if (formattedBufferContent) {
        delete formattedBufferContent;
        formattedBufferContent = 0;
    }
    buffer.close();
    buffer.setBuffer(0);
}

qint64 MsgPartReply::bytesAvailable() const
{
    //    qDebug() << "[MsgPartReply]" << __func__ << buffer.bytesAvailable();
    return buffer.bytesAvailable() + QNetworkReply::bytesAvailable();
}

qint64 MsgPartReply::readData(char *data, qint64 maxlen)
{
    //    qDebug() << "[MsgPartReply]" << __func__ << "Reading data. MaxLen: " << maxlen;
    return buffer.read(data, maxlen);
}

void MsgPartReply::failed(const QString &reason)
{
    setError(TimeoutError, reason);
    setFinished(true);
    emit error(TimeoutError);
    emit finished();
    return;
}

void MsgPartReply::handleMsgPartAvailable(const quint64 &id, const QString &location)
{
    QMailMessageId receivedId(id);
    if (receivedId.toULongLong() != m_part->partPtr()->location().containingMessageId().toULongLong()
            || location != m_part->partPtr()->location().toString(true)) {
//        qDebug() << "[MsgPartReply]" << __func__ << "Part not available... waiting";
        return;
    }
    // refresh the pointer;
    delete m_part;
    m_part = 0;
    m_part = new Part(m_msgId, m_location);
    messageReady();
}

void MsgPartReply::handlePartFetchFailed(const quint64 &id, const QString &location)
{
    QMailMessageId receivedId(id);
    if (receivedId.toULongLong() != m_part->partPtr()->location().containingMessageId().toULongLong()
            || location != m_part->partPtr()->location().toString(true)) {
//        qDebug() << "[MsgPartReply]" << __func__ << "Part not available... waiting";
        return;
    }
    failed(QStringLiteral("Failed fetching part: %1").arg(location));
}

void MsgPartReply::messageReady()
{
    // Woohoo
    emit downloadProgress(2000, 2000);
    const QMailMessagePart *part = m_part->partPtr();
    QByteArray data;
    QMailMessageContentType ct = part->contentType();
    if (ct.type().toLower() == "text") {
        data = part->body().data().toUtf8();
    } else if (ct.type().toLower() == "image") {
        data = part->body().data(QMailMessageBody::Decoded).trimmed();
    }

    if (m_format) {
        data = Formatting::markupPlainTextToHtml(data).toUtf8();
        setHeader(QNetworkRequest::ContentTypeHeader, QString("text/html")); // can't set charset=utf-8 :-(
    } else {
        //        if (!ct.charset().isEmpty()) {
        //            setHeader(QNetworkRequest::ContentTypeHeader, QString("%1; charset=%2").arg(QString(ct.content()), QString(ct.charset())));
        //        } else {
        //            setHeader(QNetworkRequest::ContentTypeHeader, QString("%1").arg(QString(ct.content())));
        //        }
        setHeader(QNetworkRequest::ContentTypeHeader, ct.content());
    }
    buffer.close();
    formattedBufferContent = new QByteArray(data);
    buffer.setBuffer(formattedBufferContent);
    //    qDebug() << "Bytes available: " << buffer.bytesAvailable();
    buffer.open(QBuffer::ReadOnly);
    setFinished(true);
    QTimer::singleShot( 0, this, SIGNAL(readyRead()) );
    QTimer::singleShot( 0, this, SIGNAL(finished()) );
}
