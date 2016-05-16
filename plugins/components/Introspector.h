#ifndef INTROSPECTOR_H
#define INTROSPECTOR_H

#include <QObject>
#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>

class Introspector : public QObject {
    Q_OBJECT

public:
    explicit Introspector (QObject * parent = Q_NULLPTR);

    static QObject * qmlSingletonProvider (QQmlEngine * qmlEngine, QJSEngine * jsEngine);

public slots:
    bool inherits (QObject * object, QObject * reference);
    QQuickWindow * window (QQuickItem * item);
};

#endif // INTROSPECTOR_H
