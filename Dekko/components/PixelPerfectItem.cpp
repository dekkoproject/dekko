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
#include "PixelPerfectItem.h"

PixelPerfectItem::PixelPerfectItem(QQuickItem *parent): QQuickItem(parent),
    m_content(Q_NULLPTR), m_timer(this)
{
    m_timer.setInterval(50);
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &PixelPerfectItem::polish, Qt::UniqueConnection);
}

void PixelPerfectItem::componentComplete()
{
    // grab a pointer to this
    QQuickItem *ptr = this;
    // Now create a vector of all ancestors
    while (ptr != Q_NULLPTR) {
        m_ancestors.append(ptr);
        connect(ptr, &QQuickItem::xChanged, this, &PixelPerfectItem::triggerUpdate, Qt::UniqueConnection);
        connect(ptr, &QQuickItem::yChanged, this, &PixelPerfectItem::triggerUpdate, Qt::UniqueConnection);
        ptr = ptr->parentItem();
    }
    // repaint
    polish();
}

void PixelPerfectItem::setContent(QQuickItem *item)
{
    if (m_content == item) {
        return;
    }
    m_content = item;
    emit contentChanged(item);
}

void PixelPerfectItem::updatePolish()
{
    if (m_content == Q_NULLPTR) {
        return;
    }
    QPointF absolute;
    for (QVector<QQuickItem *>::const_iterator it = m_ancestors.constBegin(); it != m_ancestors.constEnd(); it++) {
        absolute += (*it)->position();
    }
    m_content->setPosition(absolute.toPoint() - absolute);
}

void PixelPerfectItem::triggerUpdate()
{
    m_timer.stop();
    m_timer.start();
}

