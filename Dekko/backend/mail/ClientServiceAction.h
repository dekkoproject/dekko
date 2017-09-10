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
#ifndef CLIENTSERVICEACTION_H
#define CLIENTSERVICEACTION_H

#include <QObject>
#include <QPointer>
#include <qmailserviceaction.h>
#include <qmailmessage.h>
#include <QUuid>

class ClientServiceAction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description CONSTANT)
public:
    explicit ClientServiceAction(QObject *parent = 0) : QObject(parent)
    {
        m_uid = QUuid::createUuid().toByteArray();
    }

    enum ActionType {
        // Highest priority
        Immediate = 1, /** @short Triggered immediately cannot be undone */
        Undoable, /** @short Triggered immediately but can be undone within 5 seconds */
        Silent /** @short Queued and be processed in the near future; */
    };

    enum ServiceAction {
        CopyAction,
        MoveAction,
        DeleteAction,
        FlagAction,
        RetrieveAction,
        RetrievePartAction,
        SearchAction,
        SendAction,
        StandardFoldersAction,
        StorageAction,
        TransmitAction,
        ExportAction,
        OutboxAction,
        SyncAccountAction,
        SyncFolderAction,
        CreateStandardFolders
    };

    virtual void process() = 0;
    QMailServiceAction *action() const { return m_serviceAction; }
    ActionType actionType() const { return m_actionType; }
    ServiceAction serviceActionType() const { return m_serviceActionType; }
    QString description() const { return m_description; }
    bool isRunning() const { return m_serviceAction ? m_serviceAction->isRunning() : false; }
    QByteArray uid() const { return m_uid; }
    virtual QString location() {
        // Not implemented here
        Q_ASSERT(false);
        return QString();
    }

    virtual QMailAccountId accountId() {
        Q_ASSERT(false);
        return QMailAccountId();
    }

    virtual quint64 messageId() {
        // Not implemented here
        Q_ASSERT(false);
        return 0;
    }
    virtual QMailMessageIdList messageIds() {
        Q_ASSERT(false);
        return QMailMessageIdList();
    }

    // Used by PriorityQueue
    const bool operator < (const ClientServiceAction *r) const {
        return m_actionType < r->actionType();
    }

    const bool operator == (const ClientServiceAction *r) const {
        return m_uid == r->uid();
    }

signals:
    void activityChanged(QMailServiceAction::Activity activity);

protected:
    ActionType m_actionType;
    ServiceAction m_serviceActionType;
    QString m_description;
    QPointer<QMailServiceAction> m_serviceAction;
    QByteArray m_uid;

    QMailRetrievalAction *createRetrievalAction() {
        m_serviceAction = new QMailRetrievalAction(this);
        connect(m_serviceAction, &QMailServiceAction::activityChanged, this, &ClientServiceAction::activityChanged);
        qDebug() << "Retrieval action created";
        return static_cast<QMailRetrievalAction *>(m_serviceAction.data());
    }

    QMailStorageAction *createStorageAction(){
        m_serviceAction = new QMailStorageAction(this);
        connect(m_serviceAction, &QMailServiceAction::activityChanged, this, &ClientServiceAction::activityChanged);
        return static_cast<QMailStorageAction *>(m_serviceAction.data());
    }

    QMailTransmitAction *createTransmitAction(){
        m_serviceAction = new QMailTransmitAction(this);
        connect(m_serviceAction, &QMailServiceAction::activityChanged, this, &ClientServiceAction::activityChanged);
        return static_cast<QMailTransmitAction *>(m_serviceAction.data());
    }
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

class MoveToFolderAction : public UndoableAction
{
    Q_OBJECT
    QMailMessageIdList m_ids;
    QMailFolderId m_destination;
public:
    MoveToFolderAction(QObject *parent, const QMailMessageIdList &msgIds, const QMailFolderId &destination);
    void process();
    int itemCount();
    QMailAccountIdList accountIds();
};

class MoveToStandardFolderAction : public UndoableAction
{
    Q_OBJECT
    QMailMessageIdList m_ids;
    QMailFolder::StandardFolder m_standard;
public:
    MoveToStandardFolderAction(QObject *parent, const QMailMessageIdList &msgIds, const QMailFolder::StandardFolder &folder);
    void process();
    int itemCount();
    QMailAccountIdList accountIds();
};

class CreateStandardFoldersAction : public ClientServiceAction
{
    Q_OBJECT
    QMailAccountId m_id;
public:
    CreateStandardFoldersAction(QObject *parent, const QMailAccountId &id);
    void process();
    QMailAccountId accountId() { return m_id; }
};

class EmptyTrashAction : public ClientServiceAction
{
    Q_OBJECT
    QMailAccountId m_id;
public:
    EmptyTrashAction(QObject *parent, const QMailAccountId &id);
    void process();
    QMailAccountId accountId() { return m_id; }
};

class ExportUpdatesAction : public ClientServiceAction
{
    Q_OBJECT
    QMailAccountId m_accountId;
public:
    ExportUpdatesAction(QObject *parent, const QMailAccountId &id);
    void process();
    QMailAccountId accountId() { return m_accountId; }
};

class FlagsAction : public ClientServiceAction
{
    Q_OBJECT

public:

    enum FlagType {
        FlagStarred,
        FlagRead,
        FlagTodo
    };
    enum State {
        Apply,
        Remove
    };

    FlagsAction(QObject *parent, const QMailMessageIdList &msgs,
                const FlagsAction::FlagType &flag, const FlagsAction::State &state);

    void process();
    QMailAccountIdList accountIds();

private:
    QMailMessageIdList m_idList;
    FlagType m_flag;
    State m_state;
};

class FetchMessagePartAction : public ClientServiceAction
{
    Q_OBJECT
public:
    FetchMessagePartAction(QObject *parent, const QMailMessagePart *part);
    void process();
public slots:
    QString location() { return m_location; }
    quint64 messageId() { return m_id; }
private:
    const QMailMessagePart *m_part;
    quint64 m_id;
    QString m_location;
};

class FetchMessagesAction : public ClientServiceAction
{
    Q_OBJECT
public:
    FetchMessagesAction(QObject *parent, const QMailMessageIdList &list);
    void process();
public slots:
    QMailMessageIdList messageIds() { return m_list; }
private:
    QMailMessageIdList m_list;
};

class OutboxAction : public ClientServiceAction
{
    Q_OBJECT
public:
    OutboxAction(QObject *parent, const QMailMessage &msg);

    void process();

private:
    QMailMessage m_msg;
};

class SendPendingMessagesAction : public ClientServiceAction
{
    Q_OBJECT
public:
    SendPendingMessagesAction(QObject *parent, const QMailAccountId &id);

    QMailAccountId accountId() const { return m_id; }
    void process();

signals:
    void messagesSent(const QMailMessageIdList &ids);
    void messageSendingFailed(const QMailMessageIdList &ids, QMailServiceAction::Status::ErrorCode error);
private:
    QMailAccountId m_id;
};

class AccountSyncAction : public ClientServiceAction
{
    Q_OBJECT
public:
    AccountSyncAction(QObject *parent, const QMailAccountId &id);

    QMailAccountId accountId() { return m_id; }
    void process();

private:
    QMailAccountId m_id;
};

class FolderSyncAction : public ClientServiceAction
{
    Q_OBJECT
    QMailAccountId m_id;
    QMailFolderIdList m_list;
public:
    FolderSyncAction(QObject *parent, const QMailAccountId &id, const QMailFolderIdList &folders);
    QMailAccountId accountId() { return m_id; }
    void process();
};

#endif // CLIENTSERVICEACTION_H
