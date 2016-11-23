#ifndef NOTIFYSENDSERVICE_H
#define NOTIFYSENDSERVICE_H

#include <QObject>
#include <PluginInfo.h>
#include <PluginLoader.h>
#include <QProcess>
#include <QScopedPointer>

class NotifySendService : public ServicePlugin
{
    Q_OBJECT
public:
    explicit NotifySendService(QObject *parent = 0);

    // PluginInfo interface
public:
    virtual QString pluginId() const override;
    virtual QString location() const override;
    virtual QString i18n() const override;
    virtual QVariantMap documentation() const override;

    // ServicePlugin interface
public:
    virtual void start();
    virtual void stop(){}

private:
    QProcess m_process;
};

class NotifySendPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "example.notifysend.plugin")

    // PluginInterface interface
public:
    virtual QString name() const;
    virtual QString description() const;
    virtual PluginInfo *create(QObject *parent) const;
};

#endif // NOTIFYSENDSERVICE_H
