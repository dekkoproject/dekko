#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QObject>
#include <QMap>
#include <QmlObjectListModel.h>
#include <qmailmessage.h>
#include <qmailmessagekey.h>
#include <qmailmessagesortkey.h>
#include "Message.h"

#define INCREMENT_VALUE 50

class MessageList : public QObject
{
    Q_OBJECT
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
    Q_PROPERTY(bool canLoadMore READ canLoadMore NOTIFY totalCountChanged)
    Q_PROPERTY(int currentSelectedIndex READ currentSelectedIndex WRITE setCurrentSelectedIndex NOTIFY currentSelectedIndexChanged)
//    /** @short unread count for this messagekey */
//    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(bool isInSelectionMode READ isInSelectionMode NOTIFY isInSelectionModeChanged)

//    // Selectionmode icon properties
    Q_PROPERTY(bool canSelectAll READ canSelectAll NOTIFY selectionIndexesChanged)
    Q_PROPERTY(bool canMarkSelectionRead READ canMarkSelectionAsRead NOTIFY selectionIndexesChanged)
    Q_PROPERTY(bool canMarkSelectionImportant READ canMarkSelectionImportant NOTIFY selectionIndexesChanged)

    Q_ENUMS(FilterKey)

public:
    explicit MessageList(QObject *parent = 0);

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

    int currentSelectedIndex() const;

    FilterKey filterKey() const;

signals:
    void totalCountChanged();
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

private slots:
    void handleNewMessages(const QMailMessageIdList &newList);
    void handleMessagesRemoved(const QMailMessageIdList &removedList);
    void handleUpdatedMessages(const QMailMessageIdList &updatedList);

private: // functions
    void insertMessageAt(const int &index, const QMailMessageId &id);
    void removeMessageAt(const int &index);
    void addNewMessages(const QMailMessageIdList &idList);
    void sortAndAppendNewMessages(const QMailMessageIdList &idsToAppend, const QMailMessageIdList &newIdsList);
    void removeMessages(const QMailMessageIdList &idList);
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
};

#endif // MESSAGELIST_H
