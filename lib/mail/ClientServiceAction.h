#ifndef CLIENTSERVICEACTION_H
#define CLIENTSERVICEACTION_H

#include <QObject>
#include <QPointer>
#include <qmailserviceaction.h>

class ClientServiceAction : public QObject
{
    Q_OBJECT
public:
    explicit ClientServiceAction(QObject *parent = 0) : QObject(parent)
    {
    }


    enum ActionType {
        Silent, /** @short Queued and be processed in the near future; */
        Undoable, /** @short Triggered immediately but can be undone */
        Immediate /** @short Triggered immediately cannot be undone */
    };

    enum ServiceAction {
        CopyAction,
        MoveAction,
        DeleteAction,
        FlagAction,
        RetrieveAction,
        SearchAction,
        SendAction,
        StandardFoldersAction,
        StorageAction,
        TransmitAction,
        ExportAction
    };

    virtual void process() = 0;
    QMailServiceAction *action() const { return m_serviceAction; }
    ActionType actionType() const { return m_actionType; }
    ServiceAction serviceActionType() const { return m_serviceActionType; }
    QString description() const { return m_description; }

protected:
    ActionType m_actionType;
    ServiceAction m_serviceActionType;
    QString m_description;
    QPointer<QMailServiceAction> m_serviceAction;
};

class UndoableAction : public ClientServiceAction
{
    Q_OBJECT
public:
    explicit UndoableAction(QObject *parent = 0) : ClientServiceAction(parent)
    {
        m_actionType = ActionType::Undoable;
    }

    enum ItemType { Folder, Message };
    // Number of QMail* items this action is being applied on
    virtual int itemCount() = 0;
    virtual QMailAccountIdList accountIds() = 0;
    ItemType itemType() const { return m_itemType; }

protected:
    ItemType m_itemType;
};

class DeleteMessagesAction : public UndoableAction
{
    Q_OBJECT
    QMailMessageIdList m_ids;
public:
    DeleteMessagesAction(QObject *parent, const QMailMessageIdList &msgIds);

    void process();
    int itemCount();
    QMailAccountIdList accountIds();
};

//class MoveMessagesAction : public UndoableAction
//{
//    Q_OBJECT
//    QMailMessageIdList m_ids;
//    QMailFolderId m_destination;
//public:
//    MoveMessagesAction(QObject *parent, const QMailMessageIdList &msgIds, const QMailFolderId &destination);
//    void process();
//    int itemCount();
//    QMailAccountIdList accountIds();
//};

#endif // CLIENTSERVICEACTION_H
