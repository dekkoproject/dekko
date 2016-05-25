#include "MessageList.h"
#include <algorithm>
#include <QDebug>
#include <qmailstore.h>
#include <qmaildisconnected.h>
#include "Client.h"

MessageList::MessageList(QObject *parent) : QObject(parent),
    m_model(0), m_initialized(false), m_selectionMode(false), m_currentIndex(-1), m_filter(FilterKey::All)
{
    m_model = new QQmlObjectListModel<MinimalMessage>(this);
    m_msgKey = QMailMessageKey::nonMatchingKey();
    m_sortOrder = Qt::DescendingOrder;
    m_sortKey = QMailMessageSortKey::timeStamp(m_sortOrder);
    connect(QMailStore::instance(), SIGNAL(messagesAdded(QMailMessageIdList)), this, SLOT(handleNewMessages(QMailMessageIdList)));
    connect(QMailStore::instance(), SIGNAL(messagesRemoved(QMailMessageIdList)), this, SLOT(handleMessagesRemoved(QMailMessageIdList)));
    connect(QMailStore::instance(), SIGNAL(messagesUpdated(QMailMessageIdList)), this, SLOT(handleUpdatedMessages(QMailMessageIdList)));
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
    foreach (auto msg, m_model->toList()) {
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
    foreach (auto &id, checkedIds()) {
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
    foreach (auto &id, checkedIds()) {
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

int MessageList::currentSelectedIndex() const
{
    return m_currentIndex;
}

MessageList::FilterKey MessageList::filterKey() const
{
    return m_filter;
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
        QMailMessageIdList idsToAppend;
        QMailMessageIdList newIdsList(QMailStore::instance()->queryMessages(messageListKey(), m_sortKey, m_limit));

        foreach (const QMailMessageId &id, newIdsList) {
            if (!m_idList.contains(id)) {
                idsToAppend.append(id);
            }
        }
        sortAndAppendNewMessages(idsToAppend, newIdsList);
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
        qDebug() << "Message key: " << key << " Not of type QMailMessageKey";
        m_msgKey = QMailMessageKey::nonMatchingKey();
    }
    reset();
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
    foreach(auto msg, m_model->toList()) {
        msg->setProperty("checked", Qt::Checked);
    }
    emit selectionIndexesChanged();
}

void MessageList::unselectAll()
{
    foreach(auto msg, m_model->toList()) {
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
    if (m_currentIndex == currentSelectedIndex)
        return;

    m_currentIndex = currentSelectedIndex;
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

void MessageList::handleNewMessages(const QMailMessageIdList &newList)
{
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
}

void MessageList::handleMessagesRemoved(const QMailMessageIdList &removedList)
{
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
}

void MessageList::handleUpdatedMessages(const QMailMessageIdList &updatedList)
{
    QList<int> insertIndices;
    QList<int> removeIndices;
    QList<int> updateIndices;

    // Find the updated positions for our messages
    QMailMessageKey idKey(QMailMessageKey::id((m_idList.toSet() + updatedList.toSet()).toList()));
    QMailMessageIdList newIds(QMailStore::instance()->queryMessages(messageListKey() & idKey, m_sortKey, m_limit));
    QMap<QMailMessageId, int> newPositions;

    int index = 0;
    foreach (const QMailMessageId &id, newIds) {
        newPositions.insert(id, index);
        ++index;
    }

    QMap<int, QMailMessageId> indexId;
    foreach (const QMailMessageId &id, updatedList) {
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
                if (newIndex >= m_idList.count()) {
                    qWarning() << "MessageList::handleUpdatedMessages index out of bounds" << newIndex << m_idList.count();
                } else if (newIndex > 0) {
                    if (newIds.indexOf(m_idList.at(newIndex - 1)) > newIndex) {
                        reinsert = true;
                    }
                }

                if (newIndex < m_idList.count() - 1) {
                    if (newIds.indexOf(m_idList.at(newIndex + 1)) < newIndex) {
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

    // Sort the lists to yield ascending order
    std::sort(removeIndices.begin(), removeIndices.end());
    for (int i = removeIndices.count(); i > 0; --i) {
        int index = removeIndices[i - 1];
        removeMessageAt(index);
    }

    std::sort(insertIndices.begin(), insertIndices.end());
    foreach (int index, insertIndices) {
        // Since the list is ordered, if index is bigger than the limit
        // we stop inserting
        if(m_limit && index > m_limit) {
            break;
        }
        insertMessageAt(index, indexId[index]);
    }
    // Check if we passed the model limit, if so remove exceeding messages
    if (m_limit && m_idList.count() > m_limit) {
        QMailMessageIdList idsToRemove = m_idList.mid(m_limit);
        removeMessages(idsToRemove);
    }

    std::sort(updateIndices.begin(), updateIndices.end());
    foreach (int index, updateIndices) {
        m_model->at(index)->emitMinMessageChanged();
    }
    emit canPossiblyLoadMore();
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
    QMailMessageId id(m_idList.at(index));
    m_indexMap.remove(id);
    m_idList.removeAt(index);
    m_model->remove(index);

    // Adjust the indices for the items that have been moved
    QList<QMailMessageId>::iterator it = m_idList.begin() + index, end = m_idList.end();
    for ( ; it != end; ++it) {
        m_indexMap[*it] -= 1;
    }
    emit totalCountChanged();
}

void MessageList::addNewMessages(const QMailMessageIdList &idList)
{
    // Are any of these messages members of our display set?
    // Note - we must only consider messages in the set given by (those we currently know +
    // those we have now been informed of) because the database content may have changed between
    // when this event was recorded and when we're processing the signal.

    QMailMessageKey idKey(QMailMessageKey::id(m_idList + idList));
    const QMailMessageIdList newIdsList(QMailStore::instance()->queryMessages(messageListKey() & idKey, m_sortKey, m_limit));

    sortAndAppendNewMessages(idList, newIdsList);
}

void MessageList::sortAndAppendNewMessages(const QMailMessageIdList &idsToAppend, const QMailMessageIdList &newIdsList)
{
    QList<int> insertIndices;
    QMap<QMailMessageId, int> newPositions;

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
    foreach (int index, insertIndices) {
        // Since the list is ordered, if index is bigger than the limit
        // we stop inserting
        if(m_limit && index > m_limit) {
            break;
        }
        insertMessageAt(index, indexId[index]);
    }

    // Check if we passed the model limit, if so remove exceeding messages
    if (m_limit && m_idList.count() > m_limit) {
        QMailMessageIdList idsToRemove = m_idList.mid(m_limit);
        removeMessages(idsToRemove);
    }
    emit canPossiblyLoadMore();
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

QMailMessageIdList MessageList::checkedIds()
{
    if (!m_selectionMode) {
        return QMailMessageIdList();
    }
    QMailMessageIdList checkedMsgs;
    foreach(MinimalMessage *m,m_model->toList()) {
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

        int index = 0;
        QMailMessageIdList tmpList = QMailStore::instance()->queryMessages(messageListKey(), m_sortKey, m_limit);
        Q_FOREACH(auto &id, tmpList) {
            insertMessageAt(index, id);
            ++index;
        }
        m_initialized = true;
        emit canPossiblyLoadMore();
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

