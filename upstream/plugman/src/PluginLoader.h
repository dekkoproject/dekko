#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QtPlugin>
#include "PluginInfo.h"
#include <QTranslator>



class PluginInterface
{
public:
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual PluginInfo *create(QObject *parent) const = 0;
};

Q_DECLARE_INTERFACE(PluginInterface, "PluginInterface")

class PluginLoader
{
public:
    explicit PluginLoader();
    ~PluginLoader();

    void loadPlugins(QMultiMap<QString, PluginInfo *> &registry, const QString &pluginType = QString());
    void setPluginPaths(const QStringList &paths);

protected:
    PluginInfo *parsePluginSpec(const QString &path);
    void initSettings(const QVariantMap &map);
    void loadTranslations(const QString &i18nDir);

private:
    QStringList m_pluginPaths;
    QList<QPointer<QTranslator> > m_translators;
};

#endif // PLUGINLOADER_H
