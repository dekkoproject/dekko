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
#include "SearchService.h"
#include <QDebug>
#include <QTimer>

SearchService::SearchService(QObject *parent) : QObject(parent),
    m_searchAction(new QMailSearchAction(this)), m_userTerminated(false)
{
    connect(m_searchAction.data(), &QMailSearchAction::messageIdsMatched, this, &SearchService::messageIdsMatched);
    connect(m_searchAction.data(), &QMailSearchAction::activityChanged, this, &SearchService::searchActivityChanged);
}

void SearchService::search(QueryConfiguration &config)
{
    if (config.isValid()) {
        m_queue.enqueue(SearchQuery(m_searchAction, config));
        processNewQuery();
    }
}

void SearchService::cancel()
{
    if (m_searchAction->isRunning()) {
        m_searchAction->cancelOperation();
    }
    m_queue.clear();
    m_userTerminated = true;
    emit statusChanged(Canceled, m_searchAction->status().text);
}

void SearchService::processNewQuery()
{
    if (m_searchAction->isRunning()) {
        // already one in queue so cancel & execute the next
        m_searchAction->cancelOperation();
        QTimer::singleShot(0, this, SLOT(executeNextQuery()));
    } else {
        if (m_queue.count() > 1) {
            QTimer::singleShot(0, this, SLOT(executeNextQuery()));
        } else {
            executeQuery();
        }
    }
}

void SearchService::executeNextQuery()
{
    if (m_queue.isEmpty()) {
        return;
    }
    // we don't want the current head as that has just been cancelled
    m_queue.dequeue();
    executeQuery();
}

void SearchService::executeQuery()
{
    if (m_queue.isEmpty()) {
        return;
    }
    m_queue.head().execute();
}

void SearchService::searchActivityChanged(QMailServiceAction::Activity activity)
{
    switch (activity) {
    case QMailServiceAction::Failed:
    {
        qDebug() << "Search failed - ErrorCode[" << m_searchAction->status().errorCode << "]"  << m_searchAction->status().text;
        if (!m_userTerminated) {
            emit statusChanged(Failed, m_searchAction->status().text);
        }
        reset();
        break;
    }
    case QMailServiceAction::Successful:
    {
        qDebug() << "Search Successful";
        emit statusChanged(Done);
        reset();
        break;
    }
    case QMailServiceAction::InProgress:
    {
        qDebug() << "Search In progress -" << m_searchAction->status().text;
        emit statusChanged(InProgress);
        break;
    }
    case QMailServiceAction::Pending:
    {
        qDebug() << "Search Pending -" << m_searchAction->status().text;
        emit statusChanged(InProgress);
        break;
    }
    }
}

void SearchService::reset()
{
    m_queue.clear();
    m_userTerminated = false;
}


void SearchQuery::execute()
{
    action->searchMessages(qConfig.key, qConfig.qeury, qConfig.spec, qConfig.sortKey);
}
