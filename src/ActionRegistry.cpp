#include "ActionRegistry.h"
#include <QtQml>
#include <QQmlEngine>
#include <QQmlComponent>
#include "PluginRegistry.h"

ActionRegistry::ActionRegistry(QObject *parent) : QObject(parent)
{
    connect(this, &ActionRegistry::locationChanged, this, &ActionRegistry::loadActions);
}

ActionRegistry::~ActionRegistry()
{
    qDeleteAll(m_actions);
    qDeleteAll(m_defaults);
}

QList<QObject *> ActionRegistry::actions() const
{
    return m_actions;
}

QString ActionRegistry::location() const
{
    return m_location;
}

QQmlListProperty<QObject> ActionRegistry::defaultActions()
{
    return QQmlListProperty<QObject>(this, m_defaults);
}

void ActionRegistry::setLocation(QString location)
{
    if (m_location == location)
        return;

    m_location = location;
    emit locationChanged(location);
}

void ActionRegistry::loadActions()
{
    qDebug() << "Loading actions for " << m_location;
    if (!m_actions.isEmpty()) {
        qDeleteAll(m_actions);
        m_actions.clear();
    }
    QList<QObject *> newActions;
    auto plugins = PluginRegistry::instance()->getByLocation(m_location);
    for (auto plugin : plugins) {
        qDebug() << "Found UCAction plugin " << plugin->pluginId();
        if (auto dp = qobject_cast<DekkoPlugin *>(plugin)) {
            auto engine = qmlEngine(this);
            QQmlComponent actionComponent(engine, QUrl::fromLocalFile(dp->component()));
            if (actionComponent.isError()) {
                qDebug() << "Failed loading action plugin:";
                for (auto error : actionComponent.errors()) {
                    qDebug() << error.toString();
                }
                continue;
            }
            QObject *action = actionComponent.create();
            QByteArray cname(action->metaObject()->className());
            if (cname == QByteArrayLiteral("UCAction")) {
                qDebug() << "UCAction created >> " << dp->component();
                newActions << action;
            } else {
                qWarning() << "Plugin component not of type UCAction. Not going to use this";
            }
        }
    }
    m_actions << m_defaults << newActions;
    emit actionsChanged(m_actions);
    qDebug() << "Actions loaded for " << m_location;
}

