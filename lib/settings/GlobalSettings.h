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
#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QVariantMap>
#include "SettingsObjectBase.h"

struct GlobalSettingsKeys {
    static const QString defaultsCreated;
    static const QString messageViewStyle, messageViewMonospaceFont;
    static const QString allowRemoteContent;
};

class GlobalSettings : public SettingsObjectBase
{
    Q_OBJECT
    Q_ENUMS(Keys)
public:
    explicit GlobalSettings(QObject *parent = 0);
    enum Keys {
        DefaultsCreated = 0,
        MessageViewStyle,
        MessageViewMonospaceFont,
        AllowRemoteContent
    };

    QString keyValue(Keys key);
    Q_INVOKABLE QVariant get(Keys key);
    Q_INVOKABLE void set(Keys key, const QJsonValue &value);

protected:
    virtual void createDefaultsIfNotExist() override;
};

#endif // GLOBALSETTINGS_H
