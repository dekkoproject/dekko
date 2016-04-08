#include "ForbiddenReply.h"
#include <QTimer>

ForbiddenReply::ForbiddenReply(QObject *parent):
    QNetworkReply(parent)
{
    setError(QNetworkReply::ContentOperationNotPermittedError, tr("Remote Content Is Banned"));
    setFinished(true);
    QTimer::singleShot(0, this, SLOT(slotFinish()));
}

ForbiddenReply::ForbiddenReply(QObject *parent, const QString &reason):
    QNetworkReply(parent)
{
    setError(QNetworkReply::ContentOperationNotPermittedError, reason);
    setFinished(true);
    QTimer::singleShot(0, this, SLOT(slotFinish()));
}

void ForbiddenReply::slotFinish()
{
    emit error(QNetworkReply::ContentOperationNotPermittedError);
    emit finished();
}


