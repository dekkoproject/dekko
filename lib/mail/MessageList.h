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
    /** @short Total count - this is the total available message count and not this models count as we have a limit */
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(bool canLoadMore READ canLoadMore NOTIFY totalCountChanged)
//    /** @short unread count for this messagekey */
//    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY unreadCountChanged)
    Q_PROPERTY(bool isInSelectionMode READ isInSelectionMode NOTIFY isInSelectionModeChanged)

    Q_ENUMS(Actions)
    Q_ENUMS(ReadFlag)

public:
    explicit MessageList(QObject *parent = 0);

    enum ReadFlag {
        FlagRead,
        FlagUnread
    };

    enum Actions {
        NoopAction, /** @short No operation applied */
        FlagAction, /** @short applies the \Flagged keyword */
        MoveAction, /** @short exactly what it says on the tin */
        DeleteAction, /** @short ditto */
        ReadUnreadAction /** @short applies the read unread flag */
    };

    QObject *model() const { return m_model; }

    int limit() const;
    QVariant key() const;
    Qt::SortOrder sortOrder() const;
    int totalCount();
    bool canLoadMore();
    bool isInSelectionMode() const { return m_selectionMode; }

    Q_INVOKABLE int indexOf(const quint64 &id);
    int indexOf(const QMailMessageId &id);
    Q_INVOKABLE void loadMore();

signals:
    void totalCountChanged();
    void modelChanged();
    void limitChanged(int limit);
    void messageKeyChanged();
    void sortOrderChanged();
    void isInSelectionModeChanged();
    void selectionStarted();
    void selectionEnded();

public slots:
    void setLimit(int limit);
    void setKey(const QVariant &key);
    void setSortOrder(const Qt::SortOrder &order);
    void startSelection();
    void endSelection();
    /** @short Applies the predefined actions to the multi selected items

        folderId will only be evaluated for MoveAction
    */
    void applySelectionAction(const Actions action, const ReadFlag flag = FlagRead, const int &folderId = -1);
    void markMessageRead(const int &msgId, const ReadFlag &flag);

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
    void markMessagesRead(const QMailMessageIdList &idList, const ReadFlag &flag);
    QMailMessageIdList checkedIds();
    void init();
    void reset();

private: //members
    typedef QMap<QMailMessageId, int> MessageIndexMap;

    QQmlObjectListModel<MinimalMessage> *m_model;
    QMailMessageIdList m_idList; // List if id's in our model.
    MessageIndexMap m_indexMap;
    int m_limit = 50;
    QMailMessageKey m_msgKey;
    QMailMessageSortKey m_sortKey;
    Qt::SortOrder m_sortOrder;
    bool m_initialized;
    bool m_selectionMode;

};

#endif // MESSAGELIST_H
