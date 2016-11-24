#include "UOAManager.h"
#include <QWeakPointer>

static QWeakPointer<Accounts::Manager> sharedOAManager;
QSharedPointer<Accounts::Manager> UOASharedManager::instance()
{
    QSharedPointer<Accounts::Manager> manager = sharedOAManager.toStrongRef();
    if (manager.isNull()) {
        manager = QSharedPointer<Accounts::Manager>(new Accounts::Manager);
        sharedOAManager = manager;
    }
    return manager;
}
