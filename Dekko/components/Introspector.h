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
#ifndef INTROSPECTOR_H
#define INTROSPECTOR_H

#include <QObject>
#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>

class Introspector : public QObject {
    Q_OBJECT

public:
    explicit Introspector (QObject * parent = Q_NULLPTR);

    static QObject * qmlSingletonProvider (QQmlEngine * qmlEngine, QJSEngine * jsEngine);

public slots:
    bool inherits (QObject * object, QObject * reference);
    QQuickWindow * window (QQuickItem * item);
};

#endif // INTROSPECTOR_H
