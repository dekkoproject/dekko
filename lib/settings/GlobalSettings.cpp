/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
*/
#include "GlobalSettings.h"
#include <QPointer>
#include <Paths.h>
#include "SettingsFileBase.h"

const QString GlobalSettingsKeys::defaultsCreated = QStringLiteral("defaultscreated");
// Supported styles "default" and "clean"
const QString GlobalSettingsKeys::messageViewStyle = QStringLiteral("messageview.style");
const QString GlobalSettingsKeys::allowRemoteContent = QStringLiteral("messageview.security.allowRemoteContent");
const QString GlobalSettingsKeys::messageViewMonospaceFont = QStringLiteral("messageview.useMonospaceFont");

static QPointer<SettingsFileBase> globalSettingsFile;

GlobalSettings::GlobalSettings(QObject *parent) : SettingsObjectBase(parent)
{
    if (globalSettingsFile.isNull()) {
        QString error;
        QScopedPointer<SettingsFileBase> file(new SettingsFileBase);
        QLockFile *lock = 0;
        QString globalSettingsLock = Paths::configLocationForFile("globalsettings.json.lock");
        if (Paths::checkForStaleLockFile(&lock, globalSettingsLock, error)) {
            qCritical() << error;
            qApp->quit();
        }
        globalSettingsFile = new SettingsFileBase();
        globalSettingsFile->setPath(Paths::configLocationForFile("globalsettings.json"));
    }
    setFilePath(globalSettingsFile.data());
    if (m_file) {
        setPath(QStringLiteral("default"));
        createDefaultsIfNotExist();
    }
}

QString GlobalSettings::keyValue(GlobalSettings::Keys key)
{
    switch(key) {
    case DefaultsCreated:
        return GlobalSettingsKeys::defaultsCreated;
    case MessageViewStyle:
        return GlobalSettingsKeys::messageViewStyle;
    case MessageViewMonospaceFont:
        return GlobalSettingsKeys::messageViewMonospaceFont;
    case AllowRemoteContent:
        return GlobalSettingsKeys::allowRemoteContent;
    }
    Q_UNREACHABLE();
}

QVariant GlobalSettings::get(GlobalSettings::Keys key)
{
    return read(keyValue(key)).toVariant();
}

void GlobalSettings::set(GlobalSettings::Keys key, const QJsonValue &value)
{
    write(keyValue(key), value);
}

void GlobalSettings::createDefaultsIfNotExist()
{
    if (read(keyValue(DefaultsCreated)).toBool()) {
        return;
    }
    write(keyValue(MessageViewStyle), QStringLiteral("default"));
    write(keyValue(AllowRemoteContent), true);
    write(keyValue(MessageViewMonospaceFont), false);
    write(keyValue(DefaultsCreated), true);
}


