#include "MsgPartQNAMFactory.h"
#include "MsgPartQNAM.h"

QNetworkAccessManager *MsgPartQNAMFactory::create(QObject *parent)
{
    return new MsgPartQNAM(parent);
}
