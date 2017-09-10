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
#ifndef PIXELPERFECTITEM_H
#define PIXELPERFECTITEM_H

#include <QObject>
#include <QQuickItem>
#include <QTimer>
#include <QVector>

class PixelPerfectItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *content READ content WRITE setContent NOTIFY contentChanged)
public:
    explicit PixelPerfectItem(QQuickItem *parent = 0);

    QQuickItem *content() const { return m_content; }
    void classBegin() {}
    void componentComplete();

signals:
    void contentChanged(QQuickItem *item);

public slots:
    void setContent(QQuickItem *item);

protected:
    void updatePolish();

protected slots:
    void triggerUpdate();

private:
    QQuickItem *m_content;
    QVector<QQuickItem *> m_ancestors;
    QTimer m_timer;
};

#endif // PIXELPERFECTITEM_H
