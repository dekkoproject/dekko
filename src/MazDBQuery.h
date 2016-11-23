#ifndef MAZDBQUERY_H
#define MAZDBQUERY_H

#include <QObject>
#include <QJSValue>
#include <QQmlListProperty>
#include "MazDB.h"

class QueryBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString property READ prop WRITE setProp NOTIFY queryChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY queryChanged)
    Q_PROPERTY(MatchTypes matchRule READ matchType WRITE setMatchType NOTIFY queryChanged)
    Q_PROPERTY(QJSValue comparitor READ comparitor WRITE setComparitor NOTIFY queryChanged)
    Q_ENUMS(MatchTypes)
public:
    explicit QueryBase(QObject *parent = 0);

    enum MatchTypes {
        Equals,
        NotEquals,
        LessThan,
        LessThanOrEqual,
        GreaterThan,
        GreaterThanOrEqual
    };

    QString prop() const;
    QVariant value() const;
    MatchTypes matchType() const;

    bool hasMatch(const QVariant &value);

    QJSValue comparitor() const;

signals:
    void queryChanged();

public slots:
    void setProp(QString property);
    void setValue(QVariant value);
    void setMatchType(MatchTypes matchRule);
    void setComparitor(QJSValue comparitor);

protected:
    bool compare(const QVariant &value);
private:
    QString m_property;
    QVariant m_value;
    MatchTypes m_matchRule;
    QJSValue m_comparitor;
};

#define QUERY_CLASS(name) \
    class name : public QueryBase { \
        Q_OBJECT \
    public: \
        explicit name (QObject *parent = 0) : QueryBase(parent) {}; \
    };

QUERY_CLASS(WhereQuery)
QUERY_CLASS(OrQuery)
QUERY_CLASS(AndQuery)

class Range : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString start READ start WRITE setStart NOTIFY startChanged)
    Q_PROPERTY(QString end READ end WRITE setEnd NOTIFY endChanged)
public:
    explicit Range(QObject *parent = 0);
    bool hasRange() const;
    QString start() const;
    QString end() const;

public slots:
    void setStart(QString start);
    void setEnd(QString end);

signals:
    void startChanged(QString start);
    void endChanged(QString end);

private:
    Q_DISABLE_COPY(Range)
    QString m_start;
    QString m_end;
};

class MazDBQuery : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Range *range READ range)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(QQmlListProperty<QueryBase> definition READ query)
public:
    explicit MazDBQuery(QObject *parent = 0);

    Range *range();
    int limit() const;
    QQmlListProperty<QueryBase> query();
    bool runQuery(MazDB *db, MazCallBack callback);

public slots:
    void setLimit(int limit);

signals:
    void limitChanged(int limit);

protected:
    Range m_range;
    int m_limit;
    WhereQuery *getWhereQuery();
    OrQuery *getOrQuery();
    QList<AndQuery *> getAndQueries();

private:
    Q_DISABLE_COPY(MazDBQuery)
    QList<QueryBase *> m_queries;
};



#endif // MAZDBQUERY_H
