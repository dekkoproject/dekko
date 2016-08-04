#ifndef SEARCHFILTER_H
#define SEARCHFILTER_H

#include <QObject>
#include <QSortFilterProxyModel>

class SearchFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QObject *srcModel READ srcModel WRITE setSrcModel NOTIFY srcSet)
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
    QString m_filterString;

public:
    explicit SearchFilter(QObject *parent = 0);
    QObject *srcModel() const;
    QString filterString() const;

Q_SIGNALS:
    void srcSet();
    void filterStringChanged(QString filterString);

public slots:
    void setSrcModel(QObject * srcModel);
    void setFilterString(QString filterString);

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};

#endif // SEARCHFILTER_H
