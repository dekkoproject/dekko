#ifndef MAILBOXSEARCH_H
#define MAILBOXSEARCH_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "Message.h"
#include <qmailmessage.h>
#include <qmailmessagekey.h>
#include <qmailmessagesortkey.h>
#include <qmailserviceaction.h>
#include "SearchService.h"

class MailboxSearch : public QObject
{
    Q_OBJECT
    /** @short Message model containing search results */
    Q_PROPERTY(QObject *results READ result NOTIFY modelChanged)
    /** @short Message key to perform the search against. Defaults to QMAilMessageKey::nonMatchingKey() */
    Q_PROPERTY(QVariant messageKey READ messageKey WRITE setMessageKey NOTIFY messageKeyChanged)
    /** @short Location to search messages */
    Q_PROPERTY(Location location READ location WRITE setLocation NOTIFY locationChanged)
    /** @short number of results to return */
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    /** @short Order to sort list by */
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    /** @short key to sort results by */
    Q_PROPERTY(SortBy sortBy READ sortBy WRITE setSortBy NOTIFY sortByChanged)
    /** @short Should the query include results from message bodies. note: this may be slow */
    Q_PROPERTY(bool searchBody READ searchBody WRITE setSearchBody NOTIFY searchBodyChanged)
    // Bind text field to this property
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    /** @short Search globally and only exclude messages with the QMailMessage::Removed mask */
    Q_PROPERTY(bool globalSearch READ globalSearch WRITE setGlobalSearch NOTIFY globalSearchChanged)

    Q_ENUMS(SortBy)
    Q_ENUMS(Location)

public:
    explicit MailboxSearch(QObject *parent = 0);

    enum Location {
        Local,
        Remote,
        LocalAndRemote
    };

    enum SortBy {
        Id,
        Sender,
        Subject,
        Recipients,
        TimeStamp,
        ListId
    };

    QObject *result() const;
    QVariant messageKey() const;
    void setMessageKey(QVariant &key);
    Location location() const;
    int limit() const;
    Qt::SortOrder sortOrder() const;
    SortBy sortBy() const;
    bool searchBody() const;
    QString searchQuery() const;
    bool globalSearch() const;
    QMailMessageSortKey querySortKey();
    QMailSearchAction::SearchSpecification querySpec();
    QMailMessageKey searchKey();

signals:
    void modelChanged(QObject * results);
    void messageKeyChanged();
    void locationChanged(Location location);
    void limitChanged(int limit);
    void sortOrderChanged(Qt::SortOrder sortOrder);
    void sortByChanged(SortBy sortBy);
    void searchBodyChanged(bool searchBody);
    void searchQueryChanged(QString searchQuery);
    void globalSearchChanged(bool globalSearch);
    void statusChanged(SearchService::Status status, const QString &msg);

public slots:
    /** @short Alternative option to trigger a search */
    void search(const QString &query);
    /** @short Cancel currently running search action */
    void cancelSearch();
    void setLocation(const Location &location);
    void setLimit(const int &limit);
    void setSortOrder(const Qt::SortOrder &sortOrder);
    void setSortBy(const SortBy &sortBy);
    void setSearchBody(const bool searchBody);
    void setSearchQuery(const QString &searchQuery);
    void setGlobalSearch(const bool globalSearch);

    void handleMatchedResults(const QMailMessageIdList &idList);

private:
    QQmlObjectListModel<MinimalMessage> *m_results;
    SearchService *m_service;
    QMailMessageKey m_msgKey;
    QMailMessageSortKey m_sortKey;
    Location m_location;
    int m_limit;
    Qt::SortOrder m_sortOrder;
    SortBy m_sortBy;
    bool m_searchBody;
    QString m_searchQuery;
    bool m_globalSearch;
};

#endif // MAILBOXSEARCH_H
