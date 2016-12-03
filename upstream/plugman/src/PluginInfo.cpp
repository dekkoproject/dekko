#include "PluginInfo.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QByteArray>
#include <QStringBuilder>

PluginInfo::PluginInfo(QObject *parent) : QObject(parent)
{
}


DekkoPlugin::DekkoPlugin(QObject *parent) : PluginInfo(parent)
{
}

DekkoPlugin* DekkoPlugin::fromSpec(const QString &specPath, QObject *parent)
{
    QFile specFile(specPath);
    specFile.open(QFile::ReadOnly);
    QJsonParseError error;
    QJsonDocument p_spec = QJsonDocument().fromJson(specFile.readAll(), &error);
    if (p_spec.isObject()) {
        QJsonObject spec = p_spec.object();
        DekkoPlugin *p = new DekkoPlugin(parent);
        p->setSpec(spec);
        return p;
    } else {
        qWarning() << "Plugin failed to load at >> " << specPath;
        if (!error.errorString().isEmpty()) {
            qWarning() << "Error: " << error.errorString();
        } else {
            qWarning() << "Plugin Spec not a json object!!!!!!!!";
        }
        return Q_NULLPTR;
    }
}

QString DekkoPlugin::pluginId() const
{
    return m_spec.value(QStringLiteral("id")).toString();
}

QString DekkoPlugin::location() const
{
    return m_spec.value(QStringLiteral("register")).toString();
}

QString DekkoPlugin::component()
{
    const QString path = m_pluginPath % "/" % m_spec.value(QStringLiteral("component")).toString();
    if (QFileInfo(path).exists()) {
        return path;
    } else {
        qWarning() << "Failed to find component component: " << path;
        return QString();
    }
}

QString DekkoPlugin::i18n() const
{
    return m_spec.value(QStringLiteral("i18n")).toString();
}

QVariantMap DekkoPlugin::documentation() const
{
    return m_spec.value(QStringLiteral("documentation")).toObject().toVariantMap();
}

QVariantMap DekkoPlugin::settings() const
{
    return m_spec.value(QStringLiteral("settings")).toObject().toVariantMap();
}

QString DekkoPlugin::pluginPath() const
{
    return m_pluginPath;
}

void DekkoPlugin::setSpec(const QJsonObject &spec)
{
    m_spec = spec;
}

void DekkoPlugin::setPluginDirectory(const QString &path)
{
    m_pluginPath = path;
}

ServicePlugin::ServicePlugin(QObject *parent) : PluginInfo(parent)
{
}
