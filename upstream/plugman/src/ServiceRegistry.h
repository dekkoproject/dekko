#ifndef SERVICEREGISTRY_H
#define SERVICEREGISTRY_H

#include <QObject>
#include <QProcess>
#include "PluginInfo.h"

class ServiceRegistry : public QObject
{
    Q_OBJECT
public:
    explicit ServiceRegistry(QObject *parent = 0);
    ~ServiceRegistry();

    // The location key used to filter out service plugins
    void setServiceKey(const QString &key);
    void startServices();
    void killServices();
    void clear();

signals:
    void serviceKeyChanged();

private slots:
    void loadServices();

private:
    QList<ServicePlugin *> m_services;
    QString m_key;
};

#endif // SERVICEREGISTRY_H
