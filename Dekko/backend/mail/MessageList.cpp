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
#include "MessageList.h"
#include <algorithm>
#include <QDebug>
#include <QSet>
#include <QDBusPendingReply>
#include <QDataStream>
#include <qmailstore.h>
#include <qmaildisconnected.h>
#include <QElapsedTimer>
#include <MailServiceClient.h>
#include "serviceutils.h"

Q_LOGGING_CATEGORY(D_MSG_LIST, "dekko.mail.msglist")

void MessageListWorker::updateMessages(const QMailMessageIdList &idList, const QMailMessageIdList &needsUpdate, const QMailMessageIdList &newIds, const QMap<QMailMessageId, int> &indexMap, const int &limit)
{
    QElapsedTimer timer;
    qCDebug(D_MSG_LIST) << "[MessageListWorker::updateMessages] >> Starting";
    timer.start();

    QList<int> insertIndices;
    QList<int> removeIndices;
    QList<int> updateIndices;
    QMap<QMailMessageId, int> newPositions;

    auto indexOf = [=](const QMailMessageId &id) -> int {
        QMap<QMailMessageId, int>::const_iterator it = indexMap.find(id);
        if (it != indexMap.end()) {
            return it.value();
        }
        return -1;
    };

    int index = 0;
    foreach (const QMailMessageId &id, newIds) {
        newPositions.insert(id, index);
        ++index;
    }

    QMap<int, QMailMessageId> indexId;
    foreach (const QMailMessageId &id, needsUpdate) {
        int newIndex = -1;
        QMap<QMailMessageId, int>::const_iterator it = newPositions.find(id);
        if (it != newPositions.end()) {
            newIndex = it.value();
        }

        int oldIndex(indexOf(id));
        if (oldIndex == -1) {
            // This message was not previously in our set - add it
            if (newIndex != -1) {
                insertIndices.append(newIndex);
                indexId.insert(newIndex, id);
            }
        } else {
            // We already had this message
            if (newIndex == -1) {
                removeIndices.append(oldIndex);
            } else {
                bool reinsert(false);

                // See if this item is still sorted correctly with respect to its neighbours
                if (newIndex >= idList.count()) {
                    qCWarning(D_MSG_LIST) << "MessageListWorker::updateMessages index out of bounds" << newIndex << idList.count();
                } else if (newIndex > 0) {
                    if (newIds.indexOf(idList.at(newIndex - 1)) > newIndex) {
                        reinsert = true;
                    }
                }

                if (newIndex < idList.count() - 1) {
                    if (newIds.indexOf(idList.at(newIndex + 1)) < newIndex) {
                        reinsert = true;
                    }
                }

                if (reinsert) {
                    removeIndices.append(oldIndex);
                    insertIndices.append(newIndex);
                    indexId.insert(newIndex, id);
                } else {
                    // This message is updated but has not changed position
                    updateIndices.append(newIndex);
                }
            }
        }
    }
    //    qCDebug(D_MSG_LIST) << "Remove: " << removeIndices;
    // Sort the lists to yield ascending order
    std::sort(removeIndices.begin(), removeIndices.end());
    for (int i = removeIndices.count(); i > 0; --i) {
        int index = removeIndices[i - 1];
        emit removeMessageAt(index);
    }

    //    qCDebug(D_MSG_LIST) << "Insert: " << insertIndices;
    std::sort(insertIndices.begin(), insertIndices.end());
    foreach (const int &index, insertIndices) {
        // Since the list is ordered, if index is bigger than the limit
        // we stop inserting
        if(limit && index > limit) {
            break;
        }
        emit insertMessageAt(index, indexId[index]);
    }
    // Check if we passed the model limit, if so remove exceeding messages
    if (limit && idList.count() > limit) {
        QMailMessageIdList idsToRemove = idList.mid(limit);
        emit removeMessages(idsToRemove);
    }

    //    qCDebug(D_MSG_LIST) << "Update: " << updateIndices;
    std::sort(updateIndices.begin(), updateIndices.end());
    foreach (const int &index, updateIndices) {
        emit updateMessageAt(index);
    }

    emit canPossiblyLoadMore();
    qCDebug(D_MSG_LIST) << "[MessageListWorker::updateMessages] >> Finished in: " << timer.elapsed() << "milliseconds";
}

void MessageListWorker::sortAndAppend(const QMailMessageIdList &idList, const QMailMessageIdList &idsToAppend, const QMailMessageIdList &newIdsList, const QMap<QMailMessageId, int> &indexMap, const int &limit)
{
    QElapsedTimer timer;
    qCDebug(D_MSG_LIST) << "[MessageListWorker::sortAndAppend] >> Starting";
    timer.start();
    QList<int> insertIndices;
    QMap<QMailMessageId, int> newPositions;

    auto indexOf = [=](const QMailMessageId &id) -> int {
        QMap<QMailMessageId, int>::const_iterator it = indexMap.find(id);
        if (it != indexMap.end()) {
            return it.value();
        }
        return -1;
    };

    int index = 0;
    foreach (const QMailMessageId &id, newIdsList) {
        newPositions.insert(id, index);
        ++index;
    }

    QMap<int, QMailMessageId> indexId;
    foreach (const QMailMessageId &id, idsToAppend) {
        int newIndex = -1;
        QMap<QMailMessageId, int>::const_iterator it = newPositions.find(id);
        if (it != newPositions.end()) {
            newIndex = it.value();
        }

        int oldIndex(indexOf(id));
        if (oldIndex == -1) {
            // This message was not previously in our set - add it
            if (newIndex != -1) {
                insertIndices.append(newIndex);
                indexId.insert(newIndex, id);
            }
        }
    }
    std::sort(insertIndices.begin(), insertIndices.end());
    foreach (const int &index, insertIndices) {
        // Since the list is ordered, if index is bigger than the limit
        // we stop inserting
        if(limit && index > limit) {
            break;
        }
        emit insertMessageAt(index, indexId[index]);
    }

    // Check if we passed the model limit, if so remove exceeding messages
    if (limit && idList.count() > limit) {
        QMailMessageIdList idsToRemove = idList.mid(limit);
        emit removeMessages(idsToRemove);
    }
    emit canPossiblyLoadMore();
    qCDebug(D_MSG_LIST) << "[MessageListWorker::sortAndAppend] >> Finished in: " << timer.elapsed() << "milliseconds";

}

MessageList::MessageList(QObject *parent) : QObject(parent),
    m_model(0), m_initialized(false), m_selectionMode(false), m_currentIndex(-1), m_filter(FilterKey::All), m_disableUpdates(false),
    m_needsRefresh(false), m_loading(false), m_disableRemovals(false)
{

    qRegisterMetaType<QMap<QMailMessageId, int>>("QMap<QMailMessageId, int>");

    MessageListWorker *worker = new MessageListWorker;
    worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MessageList::sortAndAppendNewMessages, worker, &MessageListWorker::sortAndAppend);
    connect(this, &MessageList::updateMessages, worker, &MessageListWorker::updateMessages);
    connect(worker, &MessageListWorker::insertMessageAt, this, &MessageList::insertMessageAt);
    connect(worker, &MessageListWorker::removeMessageAt, this, &MessageList::removeMessageAt);
    connect(worker, &MessageListWorker::updateMessageAt, this, &MessageList::updateMessageAt);
    connect(worker, &MessageListWorker::removeMessages, this, &MessageList::removeMessages);
    connect(worker, &MessageListWorker::canPossiblyLoadMore, this, &MessageList::canPossiblyLoadMore);
    m_workerThread.start();

    m_model = new QQmlObjectListModel<MinimalMessage>(this);
    m_msgKey = QMailMessageKey::nonMatchingKey();
    m_sortOrder = Qt::DescendingOrder;
    m_sortKey = QMailMessageSortKey::timeStamp(m_sortOrder);
    connect(QMailStore::instance(), SIGNAL(messagesAdded(QMailMessageIdList)), this, SLOT(handleNewMessages(QMailMessageIdList)));
    connect(QMailStore::instance(), SIGNAL(messagesRemoved(QMailMessageIdList)), this, SLOT(handleMessagesRemoved(QMailMessageIdList)));
    connect(QMailStore::instance(), SIGNAL(messagesUpdated(QMailMessageIdList)), this, SLOT(handleUpdatedMessages(QMailMessageIdList)));
}

MessageList::~MessageList()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

int MessageList::limit() const
{
    return m_limit;
}

QVariant MessageList::key() const
{
    return m_msgKey;
}

Qt::SortOrder MessageList::sortOrder() const
{
    return m_sortOrder;
}

int MessageList::totalCount()
{
    return QMailStore::instance()->countMessages(messageListKey());
}

bool MessageList::canLoadMore()
{
    if (m_model->isEmpty()) {
        return false;
    } else {
        return m_limit < totalCount();
    }
}

bool MessageList::canSelectAll()
{
    bool shouldSelectAll = false;
    foreach (const auto *msg, m_model->toList()) {
        if (msg->checked() == Qt::Unchecked) {
            shouldSelectAll = true;
            break;
        }
    }
    return shouldSelectAll;
}

bool MessageList::canMarkSelectionAsRead()
{
    bool canMarkRead = false;
    foreach (const auto &id, checkedIds()) {
        bool isRead = (QMailMessageMetaData(id).status() & QMailMessageMetaData::Read);
        if (!isRead) {
            canMarkRead = true;
            break; // No need to keep looping. one seals the deal
        }
    }
    return canMarkRead;
}

bool MessageList::canMarkSelectionImportant()
{
    bool canMarkFlagged = false;
    foreach (const auto &id, checkedIds()) {
        bool isImportant = (QMailMessageMetaData(id).status() & QMailMessageMetaData::Important);
        if (!isImportant) {
            canMarkFlagged = true;
            break; // No need to keep looping. one seals the deal
        }
    }
    return canMarkFlagged;
}

int MessageList::indexOf(const quint64 &id)
{
    return indexOf(QMailMessageId(id));
}

int MessageList::indexOf(const QMailMessageId &id)
{
    QMap<QMailMessageId, int>::const_iterator it = m_indexMap.find(id);
    if (it != m_indexMap.end()) {
        return it.value();
    }
    return -1;
}

void MessageList::loadMore()
{
    if (canLoadMore()) {
        setLimit(m_limit + INCREMENT_VALUE);
    }
}

void MessageList::refresh()
{
    qCDebug(D_MSG_LIST) << "Refreshing Message List";
    m_loading = true;
    emit loadingChanged();
    QDBusPendingReply<QList<quint64> > reply = Client::instance()->bus()->queryMessages(
                msg_key_bytes(messageListKey()),
                msg_sort_key_bytes(m_sortKey),
                m_limit);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &MessageList::refreshResponse);
}

int MessageList::currentSelectedIndex() const
{
    return m_currentIndex;
}

MessageList::FilterKey MessageList::filterKey() const
{
    return m_filter;
}

bool MessageList::disableUpdates() const
{
    return m_disableUpdates;
}

bool MessageList::disableRemovals() const
{
    return m_disableRemovals;
}

void MessageList::setLimit(int limit)
{
    if (m_limit == limit)
        return;
    if (m_limit == 0) {
        m_limit = limit;
        reset();
    } else if (m_limit > limit) {
        m_limit = limit;
        removeMessages(m_idList.mid(limit));
    } else {
        m_limit = limit;
        refresh();
    }
    emit limitChanged(limit);
}

void MessageList::setKey(const QVariant &key)
{
    if (key.value<QMailMessageKey>() == m_msgKey) {
        return;
    }
    //reset model limit
    m_limit = INCREMENT_VALUE;

    if (key.canConvert<QMailMessageKey>()) {
        m_msgKey = key.value<QMailMessageKey>();
        emit messageKeyChanged();
    } else {
        qCDebug(D_MSG_LIST) << "Message key: " << key << " Not of type QMailMessageKey";
        m_msgKey = QMailMessageKey::nonMatchingKey();
    }
    reset();
#ifdef EXPERIMENTAL_REMEMBER_SELECTED_MESSAGE
    QByteArray cacheKey;
    QDataStream keystream(&cacheKey, QIODevice::WriteOnly);
    m_msgKey.serialize<QDataStream>(keystream);
    if (m_cache.contains(cacheKey)) {
        QMailMessageId lookingFor = *m_cache.object(cacheKey);
        foreach (auto msg, m_model->toList()) {
            if (msg->messageId() == lookingFor.toULongLong()) {
                setCurrentSelectedIndex(m_model->indexOf(msg));
                return;
            }
            setCurrentSelectedIndex(-1);
        }
    }
#endif
}

void MessageList::setSortOrder(const Qt::SortOrder &order)
{
    if (order == m_sortOrder) {
        return; // already applied
    }
    m_sortOrder = order;
    m_sortKey = QMailMessageSortKey::timeStamp(m_sortOrder);
    reset();
    emit sortOrderChanged();
}

void MessageList::startSelection()
{
    m_selectionMode = true;
    emit isInSelectionModeChanged();
    emit selectionStarted();
}

void MessageList::endSelection()
{
    m_selectionMode = false;
    emit isInSelectionModeChanged();
    emit selectionEnded();
}

void MessageList::selectAll()
{
    // XXX: iterator?
    foreach(auto *msg, m_model->toList()) {
        msg->setProperty("checked", Qt::Checked);
    }
    emit selectionIndexesChanged();
}

void MessageList::unselectAll()
{
    foreach(auto *msg, m_model->toList()) {
        msg->setProperty("checked", Qt::Unchecked);
    }
    emit selectionIndexesChanged();
}

void MessageList::setChecked(const int &index, const Qt::CheckState &checkState)
{
    if (index < m_model->count()) {
        m_model->at(index)->setProperty("checked", checkState);
        emit selectionIndexesChanged();
    }
}

void MessageList::markSelectedMessagesImportant()
{
    Client::instance()->markMessagesImportant(checkedIds(), canMarkSelectionImportant());
    unselectAll();
}

void MessageList::markSelectedMessagesRead()
{
    Client::instance()->markMessagesRead(checkedIds(), canMarkSelectionAsRead());
    unselectAll();
}

void MessageList::deleteSelectedMessages()
{
    Client::instance()->deleteMessages(checkedIds());
    unselectAll();
}

void MessageList::setCurrentSelectedIndex(int currentSelectedIndex)
{
    qCDebug(D_MSG_LIST) << "CURRENT INDEX: " << currentSelectedIndex;
    if (m_currentIndex == currentSelectedIndex)
        return;

    m_currentIndex = currentSelectedIndex;

    if (currentSelectedIndex == -1) {
        emit currentSelectedIndexChanged();
        return;
    }
#ifdef EXPERIMENTAL_REMEMBER_SELECTED_MESSAGE
    if (!m_msgKey.isEmpty()) {
        QByteArray cacheKey;
        QDataStream keystream(&cacheKey, QIODevice::WriteOnly);
        m_msgKey.serialize<QDataStream>(keystream);
        if (m_cache.contains(cacheKey)) {
            m_cache.remove(cacheKey);
        }
        m_cache.insert(cacheKey, new QMailMessageId(m_model->at(m_currentIndex)->messageId()));
    }
#endif
    emit currentSelectedIndexChanged();
}

void MessageList::setFilterKey(MessageList::FilterKey filter)
{
    if (m_filter == filter)
        return;

    m_filter = filter;
    if (m_initialized) {
        reset();
    }
    emit filterKeyChanged(filter);
}

void MessageList::setDisableUpdates(bool disableUpdates)
{
    if (m_disableUpdates == disableUpdates)
        return;

    m_disableUpdates = disableUpdates;
    emit disableUpdatesChanged(disableUpdates);

    if (!m_disableUpdates && m_needsRefresh) {
//        QSet<QMailMessageId> set = QSet<QMailMessageId>::fromList(m_refreshList);
//        handleUpdatedMessages(static_cast<QMailMessageIdList>(set.toList()));
        refresh();
        m_needsRefresh = false;
    }
}

void MessageList::setDisableRemovals(bool disableRemovals)
{
    if (m_disableRemovals == disableRemovals)
        return;

    m_disableRemovals = disableRemovals;
    emit disableRemovalsChanged(disableRemovals);
}

void MessageList::handleNewMessages(const QMailMessageIdList &newList)
{
    QElapsedTimer timer;
    qCDebug(D_MSG_LIST) << "[handleNewMessages] >> Starting";

    if (m_disableUpdates) {
        m_needsRefresh = true;
        return;
    }

    timer.start();
    if (newList.isEmpty()) {
        return;
    }

    if (m_msgKey.isNonMatching()) {
        return;
    }
    if (!m_initialized) {
        init();
    }
    addNewMessages(newList);
    qCDebug(D_MSG_LIST) << "[handleNewMessages] >> Finished in: " << timer.elapsed() << "milliseconds";
}

void MessageList::handleMessagesRemoved(const QMailMessageIdList &removedList)
{
    QElapsedTimer timer;
    qCDebug(D_MSG_LIST) << "[handleMessagesRemoved] >> Starting";

    if (m_disableUpdates) {
        m_needsRefresh = true;
        //            m_refreshList << updatedList;
        return;
    }

    if (m_disableRemovals) {
        return;
    }

    timer.start();
    if (removedList.isEmpty()) {
        return;
    }

    if (m_msgKey.isNonMatching()) {
        return;
    }
    if (!m_initialized) {
        init();
    }
    removeMessages(removedList);
    qCDebug(D_MSG_LIST) << "[handleMessagesRemoved] >> Finished in: " << timer.elapsed() << "milliseconds";
}

void MessageList::handleUpdatedMessages(const QMailMessageIdList &updatedList)
{
    QElapsedTimer timer;
    qCDebug(D_MSG_LIST) << "[handleUpdatedMessages] >> Starting";

    if (m_disableUpdates) {
        m_needsRefresh = true;
        //            m_refreshList << updatedList;
        return;
    }
    timer.start();

    QMailMessageIdList needsUpdate;
    foreach(const QMailMessageId &id, updatedList) {
        if (m_idList.contains(id)) {
            needsUpdate << id;
        }
    }

    if (needsUpdate.isEmpty()) {
        qCDebug(D_MSG_LIST) << "[handleUpdatedMessages] >> No updates in this list. Finished in: " << timer.elapsed() << "milliseconds";
        return;
    }

    // Find the updated positions for our messages
    QMailMessageKey idKey(QMailMessageKey::id((m_idList.toSet() + needsUpdate.toSet()).toList()));
    QMailMessageKey msgKey = messageListKey() & idKey;
    QDBusPendingReply<QList<quint64> > reply = Client::instance()->bus()->queryMessages(msg_key_bytes(msgKey), msg_sort_key_bytes(m_sortKey), m_limit);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, [=](QDBusPendingCallWatcher *call){
        QDBusPendingReply<QList<quint64> > reply = *call;
        if (reply.isError()) {
            qCDebug(D_MSG_LIST) << "[handleUpdatedMessages] >> Reply error";
            return;
        }
        QMailMessageIdList newIds = from_dbus_msglist(reply.argumentAt<0>());

        emit updateMessages(m_idList, needsUpdate, newIds, m_indexMap, m_limit);
        call->deleteLater();
    });
    qCDebug(D_MSG_LIST) << "[handleUpdatedMessages] >> Finished in: " << timer.elapsed() << "milliseconds";
}

void MessageList::insertMessageAt(const int &index, const QMailMessageId &id)
{
    MinimalMessage *msg = new MinimalMessage();
    msg->setMessageId(id);
    connect(this, &MessageList::selectionStarted, msg, &MinimalMessage::selectionStarted);
    connect(this, &MessageList::selectionEnded, msg, &MinimalMessage::selectionEnded);
    m_model->insert(index, msg);
    // update indexes
    m_idList.insert(index, id);
    m_indexMap.insert(id, index);
    // Adjust the indices for the items that have been moved
    QList<QMailMessageId>::iterator it = m_idList.begin() + (index + 1), end = m_idList.end();
    for ( ; it != end; ++it) {
        m_indexMap[*it] += 1;
    }
    emit totalCountChanged();
}

void MessageList::removeMessageAt(const int &index)
{
    if (m_disableRemovals) {
        return;
    }
    QElapsedTimer timer;
    qCDebug(D_MSG_LIST) << "[removeMessageAt] >> Starting";
    timer.start();
    QMailMessageId id(m_idList.at(index));
    m_indexMap.remove(id);
    m_idList.removeAt(index);
    qCDebug(D_MSG_LIST) << "Removing Message:" << m_model->at(index)->messageId();
    m_model->remove(index);

    // Adjust the indices for the items that have been moved
    QList<QMailMessageId>::iterator it = m_idList.begin() + index, end = m_idList.end();
    for ( ; it != end; ++it) {
        m_indexMap[*it] -= 1;
    }
    emit totalCountChanged();
    qCDebug(D_MSG_LIST) << "[removeMessageAt] >> Finished in: " << timer.elapsed() << "milliseconds";
}

void MessageList::addNewMessages(const QMailMessageIdList &idList)
{
    QElapsedTimer timer;
    qCDebug(D_MSG_LIST) << "[addNewMessages] >> Starting";
    timer.start();
    // Are any of these messages members of our display set?
    // Note - we must only consider messages in the set given by (those we currently know +
    // those we have now been informed of) because the database content may have changed between
    // when this event was recorded and when we're processing the signal.

    QMailMessageKey idKey(QMailMessageKey::id(m_idList + idList));
    QMailMessageKey mIdKey = messageListKey() & idKey;

    QDBusPendingReply<QList<quint64> > reply = Client::instance()->bus()->queryMessages(
                msg_key_bytes(mIdKey), msg_sort_key_bytes(m_sortKey), m_limit);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, [=](QDBusPendingCallWatcher *call){
        QDBusPendingReply<QList<quint64> > reply = *call;
        if (reply.isError()) {
            qCDebug(D_MSG_LIST) << "[addNewMessages] >> Reply error";
            return;
        }
        QMailMessageIdList newIdsList = from_dbus_msglist(reply.argumentAt<0>());
        emit sortAndAppendNewMessages(m_idList, idList, newIdsList, m_indexMap, m_limit);
        call->deleteLater();
    });
    qCDebug(D_MSG_LIST) << "[addNewMessages] >> Finished in: " << timer.elapsed() << "milliseconds";
}

void MessageList::removeMessages(const QMailMessageIdList &idList)
{
    QList<int> removeIndices;
    foreach (const QMailMessageId &id, idList) {
        int index(indexOf(id));
        if (index != -1) {
            removeIndices.append(index);
        }
    }

    // Sort the indices to yield ascending order (they must be deleted in descending order!)
    std::sort(removeIndices.begin(), removeIndices.end());

    for (int i = removeIndices.count() - 1 ; i > 0; --i) {
        int index = removeIndices.at(i - 1);
        removeMessageAt(index);
    }
}

void MessageList::updateMessageAt(const int &index)
{
    qCDebug(D_MSG_LIST) << "Message Updated: " << m_model->at(index)->messageId();
    m_model->at(index)->emitMinMessageChanged();
}

void MessageList::refreshResponse(QDBusPendingCallWatcher *call)
{
    qCDebug(D_MSG_LIST) << "[MessageList::refreshResponse] >> Started";
    QDBusPendingReply<QList<quint64> > reply = *call;
    if (reply.isError()) {
        qCDebug(D_MSG_LIST) << "Reply error for refresh response";
        return;
    }
    QList<quint64> ids = reply.argumentAt<0>();
    QMailMessageIdList newIdsList = from_dbus_msglist(ids);

    QMailMessageIdList idsToAppend;
    foreach (const QMailMessageId &id, newIdsList) {
        if (!m_idList.contains(id)) {
            idsToAppend.append(id);
        }
    }
    emit sortAndAppendNewMessages(m_idList, idsToAppend, newIdsList, m_indexMap, m_limit);
    call->deleteLater();

    if (m_loading) {
        m_loading = false;
        emit loadingChanged();
    }
    qCDebug(D_MSG_LIST) << "[MessageList::refreshResponse] >> Finished";
}

void MessageList::queryMessageResponse(QDBusPendingCallWatcher *call)
{
    qCDebug(D_MSG_LIST) << "GOT DBUS QUERY RESPONSE";
    // DO NOT REMOVE!!!
    call->deleteLater();
}

QMailMessageIdList MessageList::checkedIds()
{
    if (!m_selectionMode) {
        return QMailMessageIdList();
    }
    QMailMessageIdList checkedMsgs;
    foreach(const MinimalMessage *m, m_model->toList()) {
        if (m->checked() == Qt::Checked) {
            checkedMsgs.append(QMailMessageId(m->messageId()));
        }
    }
    return checkedMsgs;
}

void MessageList::init()
{
    if (!m_initialized) {
        m_model->clear();
        m_idList.clear();
        m_indexMap.clear();

        m_loading = true;
        emit loadingChanged();

        QDBusPendingReply<QList<quint64> > reply = Client::instance()->bus()->queryMessages(
                    msg_key_bytes(messageListKey()), msg_sort_key_bytes(m_sortKey), m_limit);

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

        connect(watcher, &QDBusPendingCallWatcher::finished, [=](QDBusPendingCallWatcher *call){
            QDBusPendingReply<QList<quint64> > reply = *call;
            if (reply.isError()) {
                qCDebug(D_MSG_LIST) << "Reply error for init";
                return;
            }
            QMailMessageIdList tmpList = from_dbus_msglist(reply.argumentAt<0>());
            int index = 0;
            Q_FOREACH(const auto &id, tmpList) {
                insertMessageAt(index, id);
                ++index;
            }
            m_initialized = true;
            emit canPossiblyLoadMore();
            call->deleteLater();

            if (m_loading) {
                m_loading = false;
                emit loadingChanged();
            }
        });
    }
}

void MessageList::reset()
{
    m_initialized = false;
    m_limit = 50;
    init();
}

QMailMessageKey MessageList::messageListKey()
{
    QMailMessageKey key;
    switch (m_filter) {
    case FilterKey::All:
        return m_msgKey;
    case FilterKey::Important:
        key = QMailMessageKey::status(QMailMessage::Important, QMailDataComparator::Includes);
        break;
    case FilterKey::Unread:
        key = QMailMessageKey::status((QMailMessage::Read | QMailMessage::ReadElsewhere), QMailDataComparator::Excludes);
        break;
    case FilterKey::Replied:
        key = QMailMessageKey::status((QMailMessage::Replied | QMailMessage::RepliedAll), QMailDataComparator::Includes);
        break;
    case FilterKey::Forwarded:
        key = QMailMessageKey::status(QMailMessage::Forwarded, QMailDataComparator::Includes);
        break;
    case FilterKey::Attachments:
        key = QMailMessageKey::status(QMailMessage::HasAttachments, QMailDataComparator::Includes);
        break;
    case FilterKey::HighPriority:
        key = QMailMessageKey::status(QMailMessage::HighPriority, QMailDataComparator::Includes);
        break;
    case FilterKey::Calendar:
        key = QMailMessageKey::status(QMailMessage::CalendarInvitation, QMailDataComparator::Includes);
        break;
    case FilterKey::Todo:
        key = QMailMessageKey::status(QMailMessage::Todo, QMailDataComparator::Includes);
        break;
    }
    return m_msgKey & key;
}



