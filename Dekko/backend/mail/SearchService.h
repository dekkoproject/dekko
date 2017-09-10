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
 *
 * \ingroup group_mail
 */
class SearchService : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
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
