#include "ActionRegistry.h"
#include <QtQml>
#include <QQmlError>
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
    qDebug() << "Loading actions for: " << m_location;
    if (!m_actions.isEmpty()) {
        qDeleteAll(m_actions);
        m_actions.clear();
    }
    auto plugins = PluginRegistry::instance()->getByLocation(m_location);
    for (auto plugin : plugins) {
        if (auto dp = qobject_cast<DekkoPlugin *>(plugin)) {
            PluginIncubator *incubator = new PluginIncubator(this);
            connect(incubator, &PluginIncubator::objectReady, this, &ActionRegistry::finishLoading);
            connect(incubator, &PluginIncubator::error, this, &ActionRegistry::handleError);
            incubator->setSourceUrl(qmlEngine(this), QUrl::fromLocalFile(dp->component()));
            m_incubators << incubator;
        }
    }
    m_actions << m_defaults;
    emit actionsChanged(m_actions);
}

void ActionRegistry::finishLoading()
{
    PluginIncubator *incubator = qobject_cast<PluginIncubator *>(sender());
    if (incubator->status() == QQmlIncubator::Ready) {
        QObject *action = incubator->object();
        QByteArray cname(action->metaObject()->className());
        if (cname == QByteArrayLiteral("UCAction")) {
            qDebug() << "Finished incubating";
            m_actions << action;
            emit actionsChanged(m_actions);
            qDebug() << "Action loaded for " << m_location;
        } else {
            qWarning() << "Plugin component not of type UCAction. Not going to use this";
            action->deleteLater();
            incubator->deleteLater();
        }
    }
}

void ActionRegistry::handleError()
{
    PluginIncubator *incubator = qobject_cast<PluginIncubator *>(sender());
    for (auto error : incubator->errors()) {
        qDebug() << "Incubator Error: " << error.toString();
    }
}

