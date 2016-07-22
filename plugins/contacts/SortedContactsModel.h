#ifndef SORTEDCONTACTSMODEL_H
#define SORTEDCONTACTSMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class SortedContactsModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QObject *srcModel READ srcModel WRITE setSrcModel NOTIFY srcSet)
    Q_PROPERTY(bool emailOnly MEMBER m_emailOnly)
    bool m_emailOnly;
public:
    explicit SortedContactsModel(QObject *parent = 0);
    QObject *srcModel() const;
signals:
    void srcSet();
public slots:
    void setSrcModel(QObject * srcModel);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};

#endif // SORTEDCONTACTSMODEL_H
