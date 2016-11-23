#ifndef LISTENERREGISTRY_H
#define LISTENERREGISTRY_H

#include <QObject>
#include <QQuickItem>
#include <QQmlParserStatus>

class ListenerRegistry : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<QQuickItem> defaultListeners READ defaultListeners)
public:
    explicit ListenerRegistry(QObject *parent = 0);

    QQmlListProperty<QQuickItem> defaultListeners();

    virtual void classBegin() override {}
    virtual void componentComplete() override;
private:
    QQuickItem *createListenerFromURl(const QString &url);
    QList<QQuickItem *> m_defaults;
    QList<QQuickItem *> m_plugins;
};

#endif // LISTENERREGISTRY_H
