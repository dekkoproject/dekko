#ifndef MAZDBQUERYENGINE_H
#define MAZDBQUERYENGINE_H

#include <QObject>
#include <QStringList>
#include <QQmlParserStatus>
#include <QJSValue>
#include "MazDBQuery.h"

class MazDBQueryEngine : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(MazDBQuery* query READ query WRITE setQuery NOTIFY queryChanged)
public:
    explicit MazDBQueryEngine(QObject *parent = 0);

    MazDBQuery* query() const;
    QString source() const;

    bool queryStream(MazCallBack callback);
    Q_INVOKABLE bool queryStream(QJSValue callback);

signals:
    void queryChanged(MazDBQuery* query);
    void sourceChanged();

public slots:
    void setSource(const QString &src);
    void setQuery(MazDBQuery* query);
    void close() { m_db->close(); }

protected:
    virtual void classBegin() override {}
    virtual void componentComplete() override { }

private:
    Q_DISABLE_COPY(MazDBQueryEngine)

    MazDB *m_db;
    MazDBQuery* m_query;
};

#endif // MAZDBQUERYENGINE_H
