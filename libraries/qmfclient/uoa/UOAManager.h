#ifndef UOAMANAGER_H
#define UOAMANAGER_H

#include <QObject>
#include <qglobal.h>
#include <qmailglobal.h>
#include <QSharedPointer>
#include <Accounts/Manager>

class QMF_EXPORT UOASharedManager
{
public:
    static QSharedPointer<Accounts::Manager> instance();
};

#endif // UOAMANAGER_H
