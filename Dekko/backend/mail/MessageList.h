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
#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QLoggingCategory>
#include <QObject>
#include <QMap>
#include <QCache>
#include <QmlObjectListModel.h>
#include <QThread>
#include <QDBusPendingCallWatcher>
#include <qmailmessage.h>
#include <qmailmessagekey.h>
#include <qmailmessagesortkey.h>
#include "Message.h"

#define INCREMENT_VALUE 50
// Remember the selected message for each key using QCache
// it works but it's really bogging down qml. Bindings need optimizing.
//#define EXPERIMENTAL_REMEMBER_SELECTED_MESSAGE

Q_DECLARE_LOGGING_CATEGORY(D_MSG_LIST)

class MessageListWorker : public QObject
{
    Q_OBJECT

signals:
    void insertMessageAt(const int &index, const QMailMessageId &id);
    void removeMessageAt(const int &index);
    void updateMessageAt(const int &index);
    void removeMessages(const QMailMessageIdList &msgIds);
    void canPossiblyLoadMore();
public slots:

    void updateMessages(
            const QMailMessageIdList &idList,
            const QMailMessageIdList &needsUpdate,
            const QMailMessageIdList &newIds,
            const QMap<QMailMessageId, int> &indexMap,
            const int& limit);

    void sortAndAppend(
            const QMailMessageIdList &idList,
            const QMailMessageIdList &idsToAppend,
            const QMailMessageIdList &newIdsList,
            const QMap<QMailMessageId, int> &indexMap,
            const int& limit);
};

class MessageList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    /** @short model containing messages in this list */
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
    /** @short Number of messages to show in the list */
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    /** @short MessageKey for this message set */
    Q_PROPERTY(QVariant messageKey READ key WRITE setKey NOTIFY messageKeyChanged)
    /** @short Sort order on DateTime */
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    /** @short Filter applied to mailstore query */
    Q_PROPERTY(FilterKey filter READ filterKey WRITE setFilterKey NOTIFY filterKeyChanged)
    /** @short Total count - this is the total available message count and not this models count as we have a limit */
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(bool canLoadMore READ canLoadMore NOTIFY canPossiblyLoadMore)
    Q_PROPERTY(int currentSelectedIndex READ currentSelectedIndex WRITE setCurrentSelectedIndex NOTIFY currentSelectedIndexChanged)
//    /** @short unread count for this messagekey */
//    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(bool isInSelectionMode READ isInSelectionMode NOTIFY isInSelectionModeChanged)

//    // Selectionmode icon properties
    Q_PROPERTY(bool canSelectAll READ canSelectAll NOTIFY selectionIndexesChanged)
    Q_PROPERTY(bool canMarkSelectionRead READ canMarkSelectionAsRead NOTIFY selectionIndexesChanged)
    Q_PROPERTY(bool canMarkSelectionImportant READ canMarkSelectionImportant NOTIFY selectionIndexesChanged)

    Q_PROPERTY(bool disableUpdates READ disableUpdates WRITE setDisableUpdates NOTIFY disableUpdatesChanged)
    Q_PROPERTY(bool disableRemovals READ disableRemovals WRITE setDisableRemovals NOTIFY disableRemovalsChanged)

    Q_ENUMS(FilterKey)

public:
    explicit MessageList(QObject *parent = 0);
    ~MessageList();

    enum FilterKey {
        All,
        Unread,
        Important,
        Replied,
        Forwarded,
        Attachments,
        HighPriority,
        Calendar,
        Todo
    };

    QObject *model() const { return m_model; }

    bool loading() const { return m_loading; }
    int limit() const;
    QVariant key() const;
    Qt::SortOrder sortOrder() const;
    int totalCount();
    bool canLoadMore();
    bool isInSelectionMode() const { return m_selectionMode; }
    bool canSelectAll();
    bool canMarkSelectionAsRead();
    bool canMarkSelectionImportant();

    Q_INVOKABLE int indexOf(const quint64 &id);
    int indexOf(const QMailMessageId &id);
    Q_INVOKABLE void loadMore();
    Q_INVOKABLE void refresh();

    int currentSelectedIndex() const;

    FilterKey filterKey() const;

    bool disableUpdates() const;

    bool disableRemovals() const;

signals:
    void loadingChanged();
    void totalCountChanged();
    void canPossiblyLoadMore();
    void modelChanged();
    void limitChanged(int limit);
    void messageKeyChanged();
    void sortOrderChanged();
    void isInSelectionModeChanged();
    void selectionStarted();
    void selectionEnded();
    void selectionIndexesChanged();

    void currentSelectedIndexChanged();

    void filterKeyChanged(FilterKey filter);

    void disableUpdatesChanged(bool disableUpdates);

    void sortAndAppendNewMessages(
            const QMailMessageIdList &idList,
            const QMailMessageIdList &idsToAppend,
            const QMailMessageIdList &newIdsList,
            const QMap<QMailMessageId, int> &indexMap,
            const int& limit);

    void updateMessages(
            const QMailMessageIdList &idList,
            const QMailMessageIdList &needsUpdate,
            const QMailMessageIdList &newIds,
            const QMap<QMailMessageId, int> &indexMap,
            const int& limit);

    void disableRemovalsChanged(bool disableRemovals);

public slots:
    void setLimit(int limit);
    void setKey(const QVariant &key);
    void setSortOrder(const Qt::SortOrder &order);

    // Multiselect options
    /** @short Enter selection mode */
    void startSelection();
    /** @short Exit selection mode */
    void endSelection();
    /** @short Select all messages in model */
    void selectAll();
    /** @short Unselects all messages in model */
    void unselectAll();
    /** @short Set checked value at index */
    void setChecked(const int &index, const Qt::CheckState &checkState);
    void markSelectedMessagesImportant();
    void markSelectedMessagesRead();
    void deleteSelectedMessages();
//    void moveSelectedMessages()
//    void copySelectedMessages()

    void setCurrentSelectedIndex(int currentSelectedIndex);

    void setFilterKey(FilterKey filter);

    void setDisableUpdates(bool disableUpdates);

    void setDisableRemovals(bool disableRemovals);

private slots:
    void handleNewMessages(const QMailMessageIdList &newList);
    void handleMessagesRemoved(const QMailMessageIdList &removedList);
    void handleUpdatedMessages(const QMailMessageIdList &updatedList);

    void insertMessageAt(const int &index, const QMailMessageId &id);
    void removeMessageAt(const int &index);
    void addNewMessages(const QMailMessageIdList &idList);
    void removeMessages(const QMailMessageIdList &idList);
    void updateMessageAt(const int &index);

    void refreshResponse(QDBusPendingCallWatcher *call);
    void queryMessageResponse(QDBusPendingCallWatcher *call);
private:
    QMailMessageIdList checkedIds();
    void init();
    void reset();

private: //members
    typedef QMap<QMailMessageId, int> MessageIndexMap;

    QMailMessageKey messageListKey();

    QQmlObjectListModel<MinimalMessage> *m_model;
    QMailMessageIdList m_idList; // List if id's in our model.
    MessageIndexMap m_indexMap;
    int m_limit = 50;
    QMailMessageKey m_msgKey;
    QMailMessageSortKey m_sortKey;
    Qt::SortOrder m_sortOrder;
    bool m_initialized;
    bool m_selectionMode;
    int m_currentIndex;
    FilterKey m_filter;
#ifdef EXPERIMENTAL_REMEMBER_SELECTED_MESSAGE
    QCache<QByteArray, QMailMessageId> m_cache;
#endif
    bool m_disableUpdates;
    bool m_needsRefresh;
    QMailMessageIdList m_refreshList;
    QThread m_workerThread;
    bool m_loading;
    bool m_disableRemovals;
};

#endif // MESSAGELIST_H
