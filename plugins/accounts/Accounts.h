#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <QObject>
#include <QmlObjectListModel.h>

/** @short Accounts class is the main interface for accessing all account related activities.
 *
 * It includes a child "model" property which provides a QList style model that can be used to access specific account actions
 */
class Accounts : public QObject
{
    Q_OBJECT
public:
    explicit Accounts(QObject *parent = 0);

signals:

public slots:
};

#endif // ACCOUNTS_H
