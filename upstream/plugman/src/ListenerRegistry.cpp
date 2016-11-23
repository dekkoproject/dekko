#include "ListenerRegistry.h"
#include "PluginRegistry.h"

ListenerRegistry::ListenerRegistry(QObject *parent) : QObject(parent)
{
}

QQmlListProperty<QQuickItem> ListenerRegistry::defaultListeners()
{
    return QQmlListProperty<QQuickItem>(this, m_defaults);
}

QQuickItem *ListenerRegistry::createListenerFromURl(const QString &url)
{
    if (url.isEmpty()) {
        qWarning() << "Invalid component url";
        return Q_NULLPTR;
    }
    auto engine = qmlEngine(this);
    QQmlComponent itemComponent(engine, QUrl::fromLocalFile(url));
    if (itemComponent.isError()) {
        for (auto error : itemComponent.errors()) {
            qDebug() << "Failed loading plugin with error:";
            qDebug() << error.toString();
        }
        return Q_NULLPTR;
    }
    return qobject_cast<QQuickItem *>(itemComponent.create());
}

void ListenerRegistry::componentComplete()
{
    auto plugins = PluginRegistry::instance()->getByLocation(QStringLiteral("Dekko::Listener"));
    for (auto plugin : plugins) {
        if (auto dp = qobject_cast<DekkoPlugin *>(plugin)) {
            qDebug() << "Loading plugin: " << dp->pluginId();
            QQuickItem *item = createListenerFromURl(dp->component());
            if (item) {
                QByteArray cname(item->metaObject()->className());
                if (cname == QByteArrayLiteral("QFAppListener")) {
                    m_plugins << item;
                } else {
                    qWarning() << "Listener Plugin not of type QFApplIstener. Instead it's" << cname;
                    item->deleteLater();
                }
            }
        }
    }
}

