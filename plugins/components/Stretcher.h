#ifndef STRETCHER_H
#define STRETCHER_H

#include <QObject>
#include <QQuickItem>

class Stretcher : public QQuickItem
{
    Q_OBJECT
public:
    explicit Stretcher (QQuickItem * parent = Q_NULLPTR) : QQuickItem(parent) {
        setImplicitHeight(-1);
        setImplicitWidth(-1);
    }
};

#endif // STRETCHER_H
