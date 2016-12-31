#ifndef IDENTITY_H
#define IDENTITY_H

#include <QObject>
#include <Helpers.h>
#include <QVariantMap>
#include <qmailaddress.h>

class Identity : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY(quint64, accountId)
    QML_WRITABLE_PROPERTY(quint64, parentId)
    QML_WRITABLE_PROPERTY(bool, isDefault)
    QML_WRITABLE_PROPERTY(QString, name)
    QML_WRITABLE_PROPERTY(QString, email)
    QML_WRITABLE_PROPERTY(QString, replyTo)
    QML_WRITABLE_PROPERTY(QString, signature)
    Q_PROPERTY(QString initials READ initials NOTIFY nameChanged)
    Q_DISABLE_COPY(Identity)
public:
    explicit Identity(QObject *parent = 0);

    static Identity *fromMap(QObject *p, const QVariantMap &map);

    QString initials();
    QMailAddress fromAddress();

public slots:
    QVariantMap toMap();
};

#endif // IDENTITY_H
