#include "SortedContactsModel.h"
#include <QDebug>
#include <Helpers.h>
#include "Contact.h"

SortedContactsModel::SortedContactsModel(QObject *parent) : QSortFilterProxyModel(parent),
    m_emailOnly(false)
{
    setDynamicSortFilter(true);
    setSortRole(Qt::UserRole);
}

QObject *SortedContactsModel::srcModel() const
{
    return sourceModel();
}

void SortedContactsModel::setSrcModel(QObject *srcModel)
{
    auto *model = static_cast<QAbstractItemModel *>(srcModel);
    Q_ASSERT(model);
    setSourceModel(model);
    invalidate();
    sort(0);
    emit srcSet();
}

bool SortedContactsModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!m_emailOnly) {
        return true;
    }
    QModelIndex srcIndex = sourceModel()->index(source_row, 0);
    Contact *contact = static_cast<Contact *>(srcIndex.data(Qt::UserRole).value<Contact *>());
    return contact->hasEmailAddress();
}

bool SortedContactsModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    Contact *left = static_cast<Contact *>(source_left.data(Qt::UserRole).value<Contact *>());
    Contact *right = static_cast<Contact *>(source_right.data(Qt::UserRole).value<Contact *>());
#if 0
    qDebug() << "+++++++++++++++++++++++++++++++++++";
    qDebug() << "LEFT" << left->sortString();
    qDebug() << "RIGHT" << right->sortString();
    qDebug() << "ACCEPTED:" << (left->sortString() < right->sortString());
    qDebug() << "+++++++++++++++++++++++++++++++++++";
#endif
    // we want all unknown contacts at the end of the list.
    // ie contacts with no sortable properties.
    if (left->sortString() == -1) {
        return false;
    }
    return left->sortString() < right->sortString();
}
