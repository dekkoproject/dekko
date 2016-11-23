#ifndef MAZDBSORTPROXY_H
#define MAZDBSORTPROXY_H

#include <QObject>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QJSValue>

class MazDBListModel;
class MazDBSortProxy : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ sourceModel WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString sortBy READ sortBy WRITE setSortBy NOTIFY sortByChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(QJSValue sortCallBack READ sortCallBack WRITE setSortCallBack NOTIFY sortCallBackChanged)

public:
    explicit MazDBSortProxy(QObject *parent = 0);
    virtual QHash<int, QByteArray> roleNames() const override;

    QString sortBy() const;

    QJSValue sortCallBack() const;

signals:
    void modelChanged();
    void sortByChanged(QString sortBy);
    void sortOrderChanged();

    void sortCallBackChanged(QJSValue sortCallBack);

public slots:
    void setModel(QAbstractItemModel* model);
    void setSortBy(QString sortBy);
    void setSortOrder(Qt::SortOrder order);
    void setSortCallBack(QJSValue sortCallBack);

protected:
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    int roleFromName(const QString &role) const;
    QString m_sortBy;
    Qt::SortOrder m_sortOrder;
    QJSValue m_sortCallBack;
};

#endif // MAZDBSORTPROXY_H
