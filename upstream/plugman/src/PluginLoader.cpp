#include "PluginLoader.h"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPluginLoader>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QLibrary>
#include <QStringBuilder>

PluginLoader::PluginLoader()
{
    // by default only look next to the app binary for a plugins folder
    m_pluginPaths << qApp->applicationDirPath() % "/plugins";
}

PluginLoader::~PluginLoader()
{
    qDeleteAll(m_translators);
    m_translators.clear();
}

void PluginLoader::loadPlugins(QMultiMap<QString, PluginInfo *> &registry, const QString &pluginType)
{
    qDebug() << "Loading plugins....";
    QStringList dirs;
    dirs << qApp->applicationDirPath() % "/plugins";
    dirs << m_pluginPaths;
    QString pluginEnv = QString(qgetenv("DEKKO_PLUGINS"));
    if (!pluginEnv.isEmpty()) {
        dirs << pluginEnv;
    }

    // First iterate over library plugins
    QStringList pluginPaths;

    qDebug() << "Looking for library plugins";
    Q_FOREACH(const QString &directory, dirs) {
        QDir dir(directory);
        Q_FOREACH(const QString &file, dir.entryList()) {
            const QString abs = QFileInfo(dir.absoluteFilePath(file)).canonicalFilePath();
            if (pluginPaths.contains(abs)) {
                continue;
            }
            pluginPaths << abs;
            if (!QLibrary::isLibrary(abs)) {
                continue;
            }
            QPluginLoader *l = new QPluginLoader(abs);
            if (l->load()) {
                if (auto plugin = qobject_cast<PluginInterface *>(l->instance())) {
                    if (PluginInfo *p = plugin->create(Q_NULLPTR)) {
                        qDebug() << "+---------------------------------+";
                        qDebug() << "Library plugin found >> " << plugin->name();
                        qDebug() << "Registered >> " << p->location();
                        qDebug() << "+---------------------------------+";
                        registry.insert(p->location(), p);
                        if (!p->i18n().isEmpty()) {
                            // we also load the translations for this plugin now
                            loadTranslations(directory % "/" % p->i18n());
                        }
                    }
                }
            }
            l->deleteLater();
        }
    }

    qDebug() << "Looking for dekko spec plugins";
    // Now scan for any *.dekko-plugin specs and generate a DekkoPlugin
    pluginPaths.clear();

    Q_FOREACH(const QString &directory, dirs) {
        QDirIterator it(directory, QStringList() << "*.dekko-plugin", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString path = it.next();
            if (pluginPaths.contains(path)) {
                continue;
            }
            pluginPaths << path;

            if (auto pluginInfo = DekkoPlugin::fromSpec(path)) {
                qDebug() << "+-------------------------------------+";
                qDebug() << "PluginReady: " << path;
                qDebug() << "Registered: " << pluginInfo->location();
                qDebug() << "Path: " << it.fileInfo().absolutePath();
                qobject_cast<DekkoPlugin *>(pluginInfo)->setPluginDirectory(it.fileInfo().absolutePath());
                registry.insert(pluginInfo->location(), pluginInfo);
                if (!pluginInfo->i18n().isEmpty()) {
                    // we also load the translations for this plugin now
                    loadTranslations(it.fileInfo().absolutePath() % "/" % pluginInfo->i18n());
                }
                qDebug() << "+-------------------------------------+";
            }
        }
    }
}

void PluginLoader::setPluginPaths(const QStringList &paths)
{
    if (!paths.isEmpty()) {
        m_pluginPaths = paths;
    }
}

void PluginLoader::loadTranslations(const QString &i18nDir)
{
    qDebug() << "Plugin declared i18n directory: " << i18nDir;
    QDir dir(i18nDir);
    if (dir.exists()) {
        qDebug() << "Plugin has i18n directory, creating a translator";
        QTranslator *tr = new QTranslator(Q_NULLPTR);
        if (tr->load(QLocale::system().name(), i18nDir)) {
            qApp->installTranslator(tr);
            m_translators.append(tr);
            qDebug() << "Translator created for directory: " << i18nDir;
        } else {
            qDebug() << "Failed creating translator for: " << i18nDir;
        }
    } else {
        qDebug() << "i18n directory doesn't exist";
    }
}


