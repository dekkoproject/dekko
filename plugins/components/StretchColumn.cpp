#include "StretchColumn.h"

#include <QElapsedTimer>
#include <QDebug>
#include <qmath.h>

StretchColumn::StretchColumn (QQuickItem * parent) : QQuickItem (parent)
  , m_spacing (0)
{ }

int StretchColumn::getSpacing (void) const {
    return m_spacing;
}

void StretchColumn::setSpacing (int spacing) {
    if (m_spacing != spacing) {
        m_spacing = spacing;
        emit spacingChanged (spacing);
    }
}

void StretchColumn::classBegin (void) {
    connect (this, &StretchColumn::childrenChanged, this, &StretchColumn::polish);
    connect (this, &StretchColumn::visibleChanged,  this, &StretchColumn::polish);
    connect (this, &StretchColumn::widthChanged,    this, &StretchColumn::polish);
    connect (this, &StretchColumn::heightChanged,   this, &StretchColumn::polish);
    connect (this, &StretchColumn::spacingChanged,  this, &StretchColumn::polish);
}

void StretchColumn::componentComplete (void) {
    polish ();
}

void StretchColumn::updatePolish (void) {
    const QList<QQuickItem *> childrenList = childItems ();
    /// find items and stretchers
    qreal tmpW = 0;
    qreal tmpH = 0;
    int nbItems   = 0;
    int nbStretch = 0;
    for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin (); it != childrenList.constEnd (); it++) {
        QQuickItem * child = (* it);
        if (child != Q_NULLPTR && !child->inherits ("QQuickRepeater") && child->isVisible ()) {
            if (child->implicitWidth () > tmpW) {
                tmpW = child->implicitWidth ();
            }
            if (child->implicitHeight () >= 0) {
                tmpH += child->implicitHeight ();
            }
            else {
                nbStretch++;
            }
            nbItems++;
        }
    }
    /// resize layout
    if (nbItems > 1) {
        tmpH += (m_spacing * (nbItems -1));
    }
    setImplicitWidth  (tmpW);
    setImplicitHeight (tmpH);
    const qreal layoutWidth  = width  ();
    const qreal layoutHeight = height ();
    const qreal autoSize = (nbStretch > 0 ? (layoutHeight - tmpH) / qreal (nbStretch) : 0);
    /// position children
    int currY = 0;
    for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin (); it != childrenList.constEnd (); it++) {
        QQuickItem * child = (* it);
        if (child != Q_NULLPTR && !child->inherits ("QQuickRepeater") && child->isVisible ()) {
            if (currY) {
                currY += m_spacing;
            }
            child->setY (currY);
            child->setWidth (layoutWidth);
            child->setHeight (child->implicitHeight () >= 0 ? child->implicitHeight () : autoSize);
            currY += child->height ();
        }
    }
}

void StretchColumn::itemChange (QQuickItem::ItemChange change, const QQuickItem::ItemChangeData & value) {
    if (change == QQuickItem::ItemChildAddedChange) {
        QQuickItem * child = value.item;
        if (child != Q_NULLPTR) {
            connect (child, &QQuickItem::visibleChanged,
                     this,  &StretchColumn::polish, Qt::UniqueConnection);
            connect (child, &QQuickItem::implicitWidthChanged,
                     this,  &StretchColumn::polish, Qt::UniqueConnection);
            connect (child, &QQuickItem::implicitHeightChanged,
                     this,  &StretchColumn::polish, Qt::UniqueConnection);
        }
    }
}

