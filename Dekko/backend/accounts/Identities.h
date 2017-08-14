#ifndef IDENTITIES_H
#define IDENTITIES_H

#include <QObject>
#include <MazDB.h>
#include <Helpers.h>
#include <qmailmessage.h>
#include "Identity.h"

typedef QList<QVariantMap> IdentityList;

class Identities : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(QString, sourcePath)
    QML_READONLY_PROPERTY(QString, errorString)
    Q_PROPERTY(int defaultIdentity READ defaultIdentity WRITE setDefaultIdentity NOTIFY defaultIdentityChanged)
    Q_ENUMS(Error)
public:
    explicit Identities(QObject *parent = 0);

    enum Error {
        InvalidId,
        InvalidParent
    };

    Q_INVOKABLE bool add(const QVariantMap &map);
    Q_INVOKABLE bool update(const QVariantMap &map);
    Q_INVOKABLE bool remove(const int &id);
    Q_INVOKABLE bool remove(const QVariantMap &map);
    Q_INVOKABLE bool removeAccount(const int &id);
    Q_INVOKABLE QVariantMap get(const int &id);
    IdentityList getAllForAccount(const int &id);

    int defaultIdentity() const;

    int determineBestIdentityFromMessage(const QMailMessage &message);

public slots:
    void setDefaultIdentity(int defaultIdentity);

signals:
    void error(Error error);
    void identitiesChanged();
    void defaultIdentityChanged(int defaultIdentity);

private:
    QString key(const QString &k);
    Q_DISABLE_COPY(Identities)
    MazDB *m_db;
    int m_defaultIdentity;
};

#endif // IDENTITIES_H
