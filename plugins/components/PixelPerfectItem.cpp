#include "PixelPerfectItem.h"

PixelPerfectItem::PixelPerfectItem(QQuickItem *parent): QQuickItem(parent),
    m_timer(this), m_content(0)
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

