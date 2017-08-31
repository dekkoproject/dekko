#include <QQmlEngine>

#if QT_VERSION < 0x050500
#include <QtQml>
namespace {
    QJSEngine *qjsEngine(const QObject *object) {
        return qmlEngine(object);
    }
}
#endif
