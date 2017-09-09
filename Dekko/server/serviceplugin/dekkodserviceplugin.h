#ifndef DEKKODSERVICEPLUGIN_H
#define DEKKODSERVICEPLUGIN_H

#include <QObject>
#include <PluginInfo.h>
#include <PluginLoader.h>
#include <QScopedPointer>

class DekkodService : public ServicePlugin
{
    Q_OBJECT
public:
    explicit DekkodService(QObject *parent = 0);

    // PluginInfo interface
public:
    virtual QString pluginId() const override;
    virtual QString location() const override;
    virtual QString i18n() const override;
    virtual QVariantMap documentation() const override;

    // ServicePlugin interface
public:
    virtual void start();
    virtual void stop();

protected:
    bool serviceFileInstalled() const;
    bool installServiceFile() const;
    bool removeServiceFile() const;
    bool serviceRunning() const;
    bool startService();
    bool restartService();
    bool stopService();

private:
    QString m_service;
    QString m_serviceFile;
};

class DekkodServicePlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "org.dekkoproject.dekkod")

    // PluginInterface interface
public:
    virtual QString name() const;
    virtual QString description() const;
    virtual PluginInfo *create(QObject *parent) const;
};

#endif // DEKKODSERVICEPLUGIN_H
