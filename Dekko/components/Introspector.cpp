/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

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
#include "Introspector.h"

#include <QMetaObject>

Introspector::Introspector (QObject * parent) : QObject (parent) { }

QObject * Introspector::qmlSingletonProvider (QQmlEngine * qmlEngine, QJSEngine * jsEngine) {
    Q_UNUSED (qmlEngine)
    Q_UNUSED (jsEngine)
    return new Introspector;
}

bool Introspector::inherits (QObject * object, QObject * reference) {
    bool ret = false;
    if (object != Q_NULLPTR && reference != Q_NULLPTR) {
        const QString objectClass    = QString::fromLatin1 (object->metaObject ()->className ());
        const QString referenceClass = QString::fromLatin1 (reference->metaObject ()->className ());
        ret = (objectClass == referenceClass);
        if (!ret) {
            ret = object->inherits (reference->metaObject ()->className ());
        }
    }
    return ret;
}

QQuickWindow * Introspector::window (QQuickItem * item) {
    return (item != Q_NULLPTR ? item->window () : Q_NULLPTR);
}
