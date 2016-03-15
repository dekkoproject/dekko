#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <QObject>
#include <QMap>
#include <QmlObjectListModel.h>
#include <qmailmessage.h>
#include <qmailmessagekey.h>
#include <qmailmessagesortkey.h>
#include "Message.h"

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
public:
    explicit MessageList(QObject *parent = 0);

    QObject *model() const { return m_model; }

    int limit() const;
    QVariant key() const;
    Qt::SortOrder sortOrder() const;

    Q_INVOKABLE int indexOf(const quint64 &id);
    int indexOf(const QMailMessageId &id);

signals:
    void modelChanged();
    void limitChanged(int limit);
    void messageKeyChanged();
    void sortOrderChanged();

public slots:
    void setLimit(int limit);
    void setKey(const QVariant &key);
    void setSortOrder(const Qt::SortOrder &order);

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

};

#endif // MESSAGELIST_H
