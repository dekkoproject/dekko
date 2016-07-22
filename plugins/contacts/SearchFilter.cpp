#include "SearchFilter.h"
#include "Contact.h"

SearchFilter::SearchFilter(QObject *parent) : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortRole(Qt::UserRole);
}

QObject *SearchFilter::srcModel() const
{
    return sourceModel();
}

QString SearchFilter::filterString() const
{
    return m_filterString;
}

void SearchFilter::setSrcModel(QObject *srcModel)
{
    auto *model = static_cast<QAbstractItemModel *>(srcModel);
    Q_ASSERT(model);
    setSourceModel(model);
    invalidateFilter();
    emit srcSet();
}

void SearchFilter::setFilterString(QString filterString)
{
    if (m_filterString == filterString)
        return;

    m_filterString = filterString.toLower();
    setFilterRegExp(m_filterString);
//    invalidateFilter();
    emit filterStringChanged(filterString);
}

bool SearchFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex srcIndex = sourceModel()->index(source_row, 0);
    Contact *contact = static_cast<Contact *>(srcIndex.data(Qt::UserRole).value<Contact *>());

    if (contact->firstname().toLower().contains(m_filterString) ||
           contact->lastname().toLower().contains(m_filterString) ) {
        return true;
    }
    return false;
}
