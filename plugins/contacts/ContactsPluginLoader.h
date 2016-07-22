#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QObject>
#include <QMap>
#include <QPluginLoader>
#include "ContactsPluginInterface.h"

class ContactsPluginLoader : public QObject
{
    Q_OBJECT
public:
    explicit ContactsPluginLoader(QObject *parent = 0);

    QMap<QString, IContactsPlugin*> contactsPlugins();

public slots:
    void loadPlugins();

private:
    void findPlugins(const QStringList &dirs, const QString &pluginType);
    void loadContactsPlugin(QObject *instance, QPluginLoader *loader);
    QMap<QString, IContactsPlugin*> m_contacts;
};

#endif // PLUGINLOADER_H
