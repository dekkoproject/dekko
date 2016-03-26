#ifndef SEARCHSERVICE_H
#define SEARCHSERVICE_H

#include <QObject>
#include <QPointer>
#include <QQueue>
#include <qmailmessage.h>
#include <qmailserviceaction.h>

struct QueryConfiguration {
    QString qeury;
    QMailMessageKey key;
    QMailSearchAction::SearchSpecification spec;
    QMailMessageSortKey sortKey;

    bool isValid() {
        return !key.isEmpty();
    }
};

struct SearchQuery {
    QPointer<QMailSearchAction> action;
    QueryConfiguration qConfig;
    SearchQuery(QMailSearchAction *sa, const QueryConfiguration &q): action(sa), qConfig(q) {}
    SearchQuery() {}
    void execute();
};

/** @short SearchService handles searching the local & remote message stores
 *
 * This has been implemented outside of ClientService to ensure results are returned in a speedy fashion
 * We make use of the second available concurrent service action we can utilize from the messageserver/mailstore
 * because the ClientService might already have a few queued actions and we would have to wait for
 * them to complete before our search get's triggered.
 *
 * We shouldn't get in the way here and just get the results back to the user asap
 */
class SearchService : public QObject
{
    Q_OBJECT
public:
    explicit SearchService(QObject *parent = 0);

    enum Status {
        Invalid,
        InProgress,
        Done,
        Failed,
        Canceled
    };

signals:
    void messageIdsMatched(const QMailMessageIdList &idList);
    void statusChanged(Status status, const QString &error = QString());

public slots:
    void search(QueryConfiguration &config);
    void cancel();

private slots:
    void processNewQuery();
    void executeNextQuery();
    void executeQuery();
    void searchActivityChanged(QMailServiceAction::Activity  activity);

private:
    void reset();

    QPointer<QMailSearchAction> m_searchAction;

    QQueue<SearchQuery> m_queue;
    bool m_userTerminated;
};

#endif // SEARCHSERVICE_H
