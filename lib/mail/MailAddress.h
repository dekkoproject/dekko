#ifndef MAILADDRESS_H
#define MAILADDRESS_H

#include <QObject>
#include <QCryptographicHash>
#include <qmailaddress.h>

class MailAddress : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY addrDataChanged)
    Q_PROPERTY(QString address READ address NOTIFY addrDataChanged)
    Q_PROPERTY(QString completeAddress READ completeAddress NOTIFY addrDataChanged)
    Q_PROPERTY(QString initials READ initials NOTIFY addrDataChanged)
    Q_PROPERTY(bool isGroup READ isGroup NOTIFY addrDataChanged)
    Q_PROPERTY(bool isNull READ isNull NOTIFY addrDataChanged)

    Q_PROPERTY(bool isContact READ isContact NOTIFY addrDataChanged)
public:
    explicit MailAddress(QObject *parent = 0);
    MailAddress(QObject *parent, const QMailAddress &address);
    MailAddress(QObject *parent, const QString &name, const QString &address);
    MailAddress(QObject *parent, const QString &address);

    QString name() const;
    QString address() const;
    QString completeAddress() const;
    QString initials();
    bool isGroup() const;
    bool isNull() const;
    bool isContact() const;

signals:
    void addrDataChanged();

public slots:
    void setAddress(const QMailAddress &address);

private:
    QMailAddress m_address;
};

#endif // MAILADDRESS_H
