/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Valério Valério <valerio.valerio@jollamobile.com>
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ssoauthplugin.h"

#include <QMap>
#include <QDebug>
#include <qmailpluginmanager.h>

#define PLUGIN_KEY "ssoauth"

typedef QMap<QString, SSOAuthService*> PluginMap;

// Load all the auth plugins into a map for quicker reference
static PluginMap initMap(QMailPluginManager& manager)
{
    PluginMap map;

    foreach (const QString &item, manager.list()) {
        QObject *instance = manager.instance(item);
        if (SSOAuthService* iface = qobject_cast<SSOAuthService*>(instance))
                map.insert(iface->key(), iface);
    }
    return map;
}

// Return a reference to a map containing all loaded plugin objects
static PluginMap& pluginMap()
{
    static QMailPluginManager pluginManager(PLUGIN_KEY);
    static PluginMap map(initMap(pluginManager));

    return map;
}

// Return the auth plugin object matching the specified ID
static SSOAuthService* mapping(const QString& key)
{
    PluginMap::ConstIterator it;
    if ((it = pluginMap().find(key)) != pluginMap().end())
        return it.value();

    qWarning() << "Failed attempt to map plugin: " << key;
    return 0;
}

SSOAuthService::SSOAuthService(QObject *parent)
    : QObject(parent)
{
}

SSOAuthService::~SSOAuthService()
{
}

/*!
    Returns a list of the keys of the installed plugins.
 */
QStringList SSOAuthFactory::keys()
{
    QStringList in;

    foreach (PluginMap::mapped_type plugin, pluginMap())
        in << plugin->key();

    return in;
}

/*!
    Creates a plugin object of the class identified by \a key.
*/
SSOAuthService* SSOAuthFactory::createService(const QString& key)
{
    if (SSOAuthService* plugin = mapping(key))
        return plugin->createService();

    return 0;
}


