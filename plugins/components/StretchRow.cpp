#include "StretchRow.h"

#include <QElapsedTimer>
#include <QDebug>
#include <qmath.h>

StretchRow::StretchRow(QQuickItem * parent) : QQuickItem (parent)
  , m_spacing (0)
{ }

int StretchRow::getSpacing(void) const {
    return m_spacing;
}

void StretchRow::setSpacing(int spacing) {
    if (m_spacing != spacing) {
        m_spacing = spacing;
        emit spacingChanged (spacing);
    }
}

void StretchRow::classBegin(void) {
    connect(this, &StretchRow::childrenChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::visibleChanged,  this, &StretchRow::polish);
    connect(this, &StretchRow::widthChanged,    this, &StretchRow::polish);
    connect(this, &StretchRow::heightChanged,   this, &StretchRow::polish);
    connect(this, &StretchRow::spacingChanged,  this, &StretchRow::polish);
}

void StretchRow::componentComplete (void) {
    polish ();
}

void StretchRow::updatePolish (void) {
    const QList<QQuickItem *> childrenList = childItems();
    /// find items and stretchers
    qreal tmpW = 0;
    qreal tmpH = 0;
    int nbItems   = 0;
    int nbStretch = 0;
    for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin (); it != childrenList.constEnd (); it++) {
        QQuickItem * child = (* it);
        if (child != Q_NULLPTR && !child->inherits("QQuickRepeater") && child->isVisible()) {
            if (child->implicitHeight () > tmpH) {
                tmpH = child->implicitHeight ();
            }
            if (child->implicitWidth () >= 0) {
                tmpW += child->implicitWidth ();
            }
            else {
                nbStretch++;
            }
            nbItems++;
        }
    }
    /// resize layout
    if (nbItems > 1) {
        tmpW += (m_spacing * (nbItems -1));
    }
    setImplicitWidth  (tmpW);
    setImplicitHeight (tmpH);
    const qreal layoutWidth = width ();
    const qreal autoSize = (nbStretch > 0 ? (layoutWidth - implicitWidth ()) / qreal (nbStretch) : 0);
    /// position children
    int currX = 0;
    for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin (); it != childrenList.constEnd (); it++) {
        QQuickItem * child = (* it);
        if (child != Q_NULLPTR && !child->inherits ("QQuickRepeater") && child->isVisible ()) {
            if (currX) {
                currX += m_spacing;
            }
            child->setX (currX);
            child->setWidth (child->implicitWidth () >= 0 ? child->implicitWidth () : autoSize);
            currX += child->width ();
        }
    }
}

void StretchRow::itemChange (QQuickItem::ItemChange change, const QQuickItem::ItemChangeData & value) {
    if (change == QQuickItem::ItemChildAddedChange) {
        QQuickItem * child = value.item;
        if (child != Q_NULLPTR) {
            connect (child, &QQuickItem::visibleChanged,
                     this,  &StretchRow::polish, Qt::UniqueConnection);
            connect (child, &QQuickItem::implicitWidthChanged,
                     this,  &StretchRow::polish, Qt::UniqueConnection);
            connect (child, &QQuickItem::implicitHeightChanged,
                     this,  &StretchRow::polish, Qt::UniqueConnection);
        }
    }
}

