/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "MailboxSearch.h"

MailboxSearch::MailboxSearch(QObject *parent) : QObject(parent),
    m_results(0), m_service(0), m_location(Local), m_limit(50), m_sortOrder(Qt::DescendingOrder),
    m_sortBy(TimeStamp), m_searchBody(false), m_globalSearch(false)
{
    m_results = new QQmlObjectListModel<MinimalMessage>(this);
    m_service = new SearchService(this);
    connect(m_service, &SearchService::statusChanged, this, &MailboxSearch::statusChanged);
    connect(m_service, &SearchService::messageIdsMatched, this, &MailboxSearch::handleMatchedResults);
    connect(this, &MailboxSearch::searchQueryChanged, [=](){
        QueryConfiguration qc;
        qc.key = QMailMessageKey(m_msgKey & searchKey());
        qc.qeury = m_searchQuery;
        qc.sortKey = querySortKey();
        qc.spec = querySpec();
        m_service->search(qc);
    });
}

QObject *MailboxSearch::result() const
{
    return m_results;
}

QVariant MailboxSearch::messageKey() const
{
    return m_msgKey;
}

void MailboxSearch::setMessageKey(QVariant &key)
{
    if (key.canConvert<QMailMessageKey>()) {
        m_msgKey = key.value<QMailMessageKey>();
        emit messageKeyChanged();
    }
}

MailboxSearch::Location MailboxSearch::location() const
{
    return m_location;
}

int MailboxSearch::limit() const
{
    return m_limit;
}

Qt::SortOrder MailboxSearch::sortOrder() const
{
    return m_sortOrder;
}

MailboxSearch::SortBy MailboxSearch::sortBy() const
{
    return m_sortBy;
}

bool MailboxSearch::searchBody() const
{
    return m_searchBody;
}

QString MailboxSearch::searchQuery() const
{
    return m_searchQuery;
}

bool MailboxSearch::globalSearch() const
{
    return m_globalSearch;
}

QMailMessageSortKey MailboxSearch::querySortKey()
{
    switch(m_sortBy) {
    case Id:
        return QMailMessageSortKey::id(m_sortOrder);
    case Sender:
        return QMailMessageSortKey::sender(m_sortOrder);
    case Subject:
        return QMailMessageSortKey::subject(m_sortOrder);
    case Recipients:
        return QMailMessageSortKey::recipients(m_sortOrder);
    case TimeStamp:
        return QMailMessageSortKey::timeStamp(m_sortOrder);
    case ListId:
        return QMailMessageSortKey::listId(m_sortOrder);
    }
    Q_UNREACHABLE();
}

QMailSearchAction::SearchSpecification MailboxSearch::querySpec()
{
    switch(m_location) {
    case LocalAndRemote: // just do local for now
    case Local:
        return QMailSearchAction::SearchSpecification::Local;
    case Remote:
        return QMailSearchAction::SearchSpecification::Remote;
    }
    Q_UNREACHABLE();
}

QMailMessageKey MailboxSearch::searchKey()
{
    QMailMessageKey searchKey;
    searchKey |= QMailMessageKey::sender(m_searchQuery, QMailDataComparator::Includes);
    searchKey |= QMailMessageKey::recipients(m_searchQuery, QMailDataComparator::Includes);
    searchKey |= QMailMessageKey::subject(m_searchQuery, QMailDataComparator::Includes);
    if (m_searchBody) {
        searchKey |= QMailMessageKey::preview(m_searchQuery, QMailDataComparator::Includes);
    }
    return searchKey;
}

void MailboxSearch::search(const QString &query)
{
    if (query.isEmpty()) {
        return;
    }
    setSearchQuery(query);
}

void MailboxSearch::cancelSearch()
{
    m_service->cancel();
    m_results->clear();
}

void MailboxSearch::setLocation(const Location &location)
{
    if (m_location == location)
        return;

    m_location = location;
    emit locationChanged(location);
}

void MailboxSearch::setLimit(const int &limit)
{
    if (m_limit == limit)
        return;

    m_limit = limit;
    emit limitChanged(limit);
}

void MailboxSearch::setSortOrder(const Qt::SortOrder &sortOrder)
{
    if (m_sortOrder == sortOrder)
        return;

    m_sortOrder = sortOrder;
    emit sortOrderChanged(sortOrder);
}

void MailboxSearch::setSortBy(const MailboxSearch::SortBy &sortBy)
{
    if (m_sortBy == sortBy)
        return;

    m_sortBy = sortBy;
    emit sortByChanged(sortBy);
}

void MailboxSearch::setSearchBody(const bool searchBody)
{
    if (m_searchBody == searchBody)
        return;

    m_searchBody = searchBody;
    emit searchBodyChanged(searchBody);
}

void MailboxSearch::setSearchQuery(const QString &searchQuery)
{
    if (m_searchQuery == searchQuery)
        return;

    m_searchQuery = searchQuery;
    emit searchQueryChanged(searchQuery);
}

void MailboxSearch::setGlobalSearch(const bool globalSearch)
{
    if (m_globalSearch == globalSearch)
        return;

    m_globalSearch = globalSearch;
    emit globalSearchChanged(globalSearch);
}

void MailboxSearch::handleMatchedResults(const QMailMessageIdList &idList)
{
    // the results are already sorted for us
    m_results->clear();
    Q_FOREACH(auto &id, idList) {
        MinimalMessage *msg = new MinimalMessage();
        msg->setMessageId(id);
        m_results->append(msg);
    }
}

