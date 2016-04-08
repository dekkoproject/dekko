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
