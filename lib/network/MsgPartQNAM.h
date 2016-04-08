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
