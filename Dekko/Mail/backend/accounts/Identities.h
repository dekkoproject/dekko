#ifndef IDENTITIES_H
#define IDENTITIES_H

#include <QObject>
#include <MazDB.h>
#include <Helpers.h>
#include "Identity.h"

class Identities : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(QString, sourcePath)
    QML_READONLY_PROPERTY(QString, errorString)
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
    Q_INVOKABLE QVariantMap get(const int &id);

signals:
    void error(Error error);

private:
    QString key(const QString &k);
    Q_DISABLE_COPY(Identities)
    MazDB *m_db;
};

#endif // IDENTITIES_H
