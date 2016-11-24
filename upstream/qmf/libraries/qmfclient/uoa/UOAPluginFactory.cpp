/**************************************************************************
   Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
#include "UOAPluginFactory.h"
#include <QWeakPointer>

static QWeakPointer<UOAPluginFactory> pluginManager;
QSharedPointer<UOAPluginFactory> UOAPluginFactory::instance()
{
    QSharedPointer<UOAPluginFactory> manager = pluginManager.toStrongRef();
    if (manager.isNull()) {
        manager = QSharedPointer<UOAPluginFactory>(new UOAPluginFactory);
        pluginManager = manager;
    }
    return manager;
}

QStringList UOAPluginFactory::services()
{
    if (!m_loader->loaded()) {
        m_loader->loadPlugins();
    }
    QStringList names;
    UOAPlugins plugins(m_loader->uoaPlugins());
    for (auto name: plugins.keys()) {
        names << name;
    }
    return names;
}

UOAService *UOAPluginFactory::createService(const QString &service, const quint32 &accountId)
{
    UOAService *s = m_loader->get(service);
    if (s) {
        return s->create(accountId);
    }
    return Q_NULLPTR;
}

UOAPlugins UOAPluginLoader::uoaPlugins()
{
    return m_plugins;
}

UOAService *UOAPluginLoader::get(const QString &service)
{
    UOAPlugins::const_iterator it;
    if ((it = m_plugins.find(service)) != m_plugins.end()) {
        return it.value();
    }
    return Q_NULLPTR;
}

void UOAPluginLoader::loadPlugins()
{
    Q_FOREACH(const QString &p, m_pluginMgr.list()) {
        QObject *i = m_pluginMgr.instance(p);
        UOAService *service = qobject_cast<UOAService *>(i);
        if (service) {
            m_plugins.insert(service->name(), service);
        }
    }
    m_loaded = true;
}
