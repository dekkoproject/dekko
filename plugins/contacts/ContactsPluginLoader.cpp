#include "ContactsPluginLoader.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <Paths.h>
#include <Helpers.h>
#include <QStringBuilder>

ContactsPluginLoader::ContactsPluginLoader(QObject *parent) : QObject(parent)
{
    DELAY_CALL_NOARG(this, loadPlugins)
}

QMap<QString, IContactsPlugin *> ContactsPluginLoader::contactsPlugins()
{
    return m_contacts;
}

void ContactsPluginLoader::loadPlugins()
{
    QStringList dirs;
    // load plugins app binary dir
    dirs << qApp->applicationDirPath() % "/plugins";
    // support plugins in .local/share/dekko.dekkoproject/plugins
    dirs << Paths::standardDataLocation() % "/plugins";
    QString dekkoPlugins = QString(qgetenv("DEKKO_PLUGINS"));
    if (dekkoPlugins.isEmpty()) {
        dekkoPlugins = QStringLiteral(DEKKO_PLUGINS_DIR);
    }
    dirs << dekkoPlugins;

    findPlugins(dirs, QStringLiteral("contacts"));
}

void ContactsPluginLoader::findPlugins(const QStringList &dirs, const QString &pluginType)
{
    QStringList pluginPaths;
    Q_FOREACH(const QString &directory, dirs) {
        QDir dir((directory % "/" % pluginType));
        Q_FOREACH(const QString &file, dir.entryList()) {
            const QString abs = QFileInfo(dir.absoluteFilePath(file)).canonicalFilePath();
            if (pluginPaths.contains(abs)) {
                continue;
            }
            pluginPaths << abs;
            if (!QLibrary::isLibrary(abs)) {
                continue;
            }
            QPluginLoader *l = new QPluginLoader(abs, this);
            if (l->load()) {
                if (pluginType == QStringLiteral("contacts")) {
                    loadContactsPlugin(l->instance(), l);
                }
            } else {
                qWarning() << "[FAILURE] << Failed loading contacts plugin >>";
                qWarning() << l->errorString();
            }
        }
    }
}

void ContactsPluginLoader::loadContactsPlugin(QObject *instance, QPluginLoader *loader)
{
    Q_ASSERT(instance);
    if (auto plugin = qobject_cast<IContactsPlugin *>(instance)) {
        const QString &name = plugin->pluginName();
        Q_ASSERT(!name.isEmpty());
        Q_ASSERT(!m_contacts.contains(name));
        m_contacts[name] = plugin;
    }
}
