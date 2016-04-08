#ifndef MSGREPLY_H
#define MSGREPLY_H

#include <QObject>
#include <QBuffer>
#include <QNetworkReply>
#include <qmailstore.h>
#include <Client.h>
#include "MsgPartQNAM.h"

class MsgReply : public QNetworkReply
{
    Q_OBJECT
public:
    MsgReply(MsgPartQNAM *parent, QMailMessageId &msgId);
    MsgReply(MsgPartQNAM *parent, QMailMessageId &msgId, const bool requiresFormatting);

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
