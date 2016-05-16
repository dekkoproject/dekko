#include "Introspector.h"

#include <QMetaObject>

Introspector::Introspector (QObject * parent) : QObject (parent) { }

QObject * Introspector::qmlSingletonProvider (QQmlEngine * qmlEngine, QJSEngine * jsEngine) {
    Q_UNUSED (qmlEngine)
    Q_UNUSED (jsEngine)
    return new Introspector;
}

bool Introspector::inherits (QObject * object, QObject * reference) {
    bool ret = false;
    if (object != Q_NULLPTR && reference != Q_NULLPTR) {
        const QString objectClass    = QString::fromLatin1 (object->metaObject ()->className ());
        const QString referenceClass = QString::fromLatin1 (reference->metaObject ()->className ());
        ret = (objectClass == referenceClass);
        if (!ret) {
            ret = object->inherits (reference->metaObject ()->className ());
        }
    }
    return ret;
}

QQuickWindow * Introspector::window (QQuickItem * item) {
    return (item != Q_NULLPTR ? item->window () : Q_NULLPTR);
}
