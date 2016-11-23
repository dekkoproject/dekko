#ifndef MAZUTILS_H
#define MAZUTILS_H
#include <QString>
#include <QVariant>
#include <QJsonObject>

namespace MazUtils {

QString variantToJson(QVariant variant);
QJsonObject variantToJsonObject(QVariant variant);
QVariant jsonToVariant(QString json);
QVariant jsonToVariant(QJsonObject object);

}

#endif // MAZUTILS_H
