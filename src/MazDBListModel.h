#ifndef MAZDBLISTMODEL_H
#define MAZDBLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>
#include <QJSValue>
#include <MazDBQuery.h>

class MazDB;

class MazDBListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(Range *range READ range)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(FilterPolicy filterPolicy READ filterPolicy WRITE setFilterPolicy NOTIFY filterPolicyChanged)
    Q_PROPERTY(QStringList customRoles READ customRoles WRITE setCustomRoles NOTIFY customRolesChanged)
    Q_PROPERTY(QJSValue rolesCallBack READ rolesCallBack WRITE setRolesCallBack NOTIFY rolesCallBackChanged)
    Q_PROPERTY(QStringList excludeKeys READ excludeKeys WRITE setExcludeKeys NOTIFY excludeKeysChanged)
    Q_ENUMS(FilterPolicy)
public:
    explicit MazDBListModel(QObject *parent = 0);

    enum Roles {
        KeyRole,
        ValueRole
    };

    enum FilterPolicy {
        ExactMatch,
        Contains
    };

    int count() const;
    Range *range();
    QString source() const;
    QString filter() const;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    FilterPolicy filterPolicy() const;
    QStringList customRoles() const;
    QStringList excludeKeys() const;
    QJSValue rolesCallBack() const;

signals:
    void sourceChanged();
    void countChanged();
    void filterChanged();
    void filterPolicyChanged(FilterPolicy filterPolicy);
    void customRolesChanged(QStringList customRoles);
    void rolesCallBackChanged(QJSValue rolesCallBack);
    void excludeKeysChanged();

public slots:
    void setSource(const QString &src);
    void setFilter(const QString &filter);
    void close();
    void clear();
    void reload();
    void setFilterPolicy(FilterPolicy filterPolicy);
    void setCustomRoles(QStringList customRoles);
    void setRolesCallBack(QJSValue rolesCallBack);
    void setExcludeKeys(QStringList keys);

private slots:
    void handleKeyValueChange(const QString &key, const QVariant &value);
    void handleKeyValueRemoved(const QString &key);

private:
    void load();
    void emitDataChanged(const QString &key);
    bool insertAllowed(const QString &key);
    MazDB *m_db;
    QStringList m_keys;
    QString m_filter;
    FilterPolicy m_filterPolicy;
    QStringList m_customRoles;
    QStringList m_excludeKeys;
    QJSValue m_rolesCallBack;
    Range m_range;
};

#endif // MAZDBLISTMODEL_H
