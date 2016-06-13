/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
#include "ClientServiceAction.h"
#include <qmaildisconnected.h>
#include <qmailmessage.h>
#include <qmailaccount.h>
#include <QDebug>

DeleteMessagesAction::DeleteMessagesAction(QObject *parent, const QMailMessageIdList &msgIds):
    UndoableAction(parent), m_ids(msgIds)
{
    m_itemType = ItemType::Message;
    m_serviceActionType = ServiceAction::DeleteAction;
    m_description = QStringLiteral("Deleting %1 messages.").arg(m_ids.count());
}

void DeleteMessagesAction::process()
{
    qDebug() << "Moving to trash" << m_ids.at(0).toULongLong();
    QMailDisconnected::moveToStandardFolder(m_ids,QMailFolder::TrashFolder);
    qDebug() << "Mark message deleted";
    QMailDisconnected::flagMessages(m_ids,QMailMessage::Trash,0,"Marking messages as deleted");
}

int DeleteMessagesAction::itemCount()
{
    return m_ids.count();
}

QMailAccountIdList DeleteMessagesAction::accountIds()
{
    QMailAccountIdList accounts;
    Q_FOREACH(auto &id, m_ids) {
        QMailAccountId accountId = QMailMessageMetaData(id).parentAccountId();
        if (!accounts.contains(accountId)) {
            accounts.append(accountId);
        }
    }
    return accounts;
}

ExportUpdatesAction::ExportUpdatesAction(QObject *parent, const QMailAccountId &id) :
    ClientServiceAction(parent), m_accountId(id)
{
    m_actionType = ActionType::Silent;
    m_serviceActionType = ServiceAction::ExportAction;
    m_description = tr("Syncing changes for %1 account").arg(QMailAccount(m_accountId).name());
}

void ExportUpdatesAction::process()
{
    qDebug() << "Exporting updates for account: " << QMailAccount(m_accountId).name();
    createRetrievalAction()->exportUpdates(m_accountId);
}

FlagsAction::FlagsAction(QObject *parent, const QMailMessageIdList &msgs,
                         const FlagsAction::FlagType &flag, const FlagsAction::State &state) :
    ClientServiceAction(parent), m_idList(msgs), m_flag(flag), m_state(state) {

    m_actionType = ActionType::Immediate;
    m_serviceActionType = ServiceAction::FlagAction;
    QString count = QString::number(m_idList.count());
    QString flagAction;
    switch (m_flag) {
    case FlagStarred:
    {
        switch (m_state) {
        case State::Apply:
            flagAction = tr("important");
            break;
        case State::Remove:
            flagAction = tr("not important");
            break;
        }
        break;
    }
    case FlagRead:
    {
        switch (m_state) {
        case State::Apply:
            flagAction = tr("read");
            break;
        case State::Remove:
            flagAction = tr("unread");
            break;
        }
        break;
    }
    case FlagTodo:
    {
        switch (m_state) {
        case State::Apply:
            flagAction = tr("as todo");
            break;
        case State::Remove:
            flagAction = tr("no loger todo");
            break;
        }
    }
    }
    m_description = tr("Marking %1 messages %2").arg(count, flagAction);
}

void FlagsAction::process()
{
    if (m_idList.isEmpty()) {
        return;
    }
    quint64 applyMask = 0;
    quint64 removeMask = 0;

    switch (m_flag) {
    case FlagStarred:
    {
        switch (m_state) {
        case State::Apply:
            applyMask = QMailMessage::Important;
            break;
        case State::Remove:
            removeMask = QMailMessage::Important;
            break;
        }
        break;
    }
    case FlagRead:
    {
        switch (m_state) {
        case State::Apply:
            applyMask = QMailMessage::Read;
            break;
        case State::Remove:
            removeMask = QMailMessage::Read;
            break;
        }
        break;
    }
    case FlagTodo:
    {
        switch (m_state) {
        case State::Apply:
            applyMask = QMailMessage::Todo;
            break;
        case State::Remove:
            removeMask = QMailMessage::Todo;
            break;
        }
        break;
    }
    }
    QMailDisconnected::flagMessages(m_idList, applyMask, removeMask, m_description);
}

QMailAccountIdList FlagsAction::accountIds()
{
    QMailAccountIdList accounts;
    Q_FOREACH(auto &id, m_idList) {
        QMailAccountId accountId = QMailMessageMetaData(id).parentAccountId();
        if (!accounts.contains(accountId)) {
            accounts.append(accountId);
        }
    }
    return accounts;
}

FetchMessagePartAction::FetchMessagePartAction(QObject *parent, const QMailMessagePart *part):
    ClientServiceAction(parent), m_part(part)
{
    m_actionType = ActionType::Immediate;
    m_serviceActionType = ServiceAction::RetrievePartAction;
    m_description = QStringLiteral("Fetching message part: %1").arg(part->location().toString(true));
}

void FetchMessagePartAction::process()
{
//    qDebug() << m_description;
    m_id = m_part->location().containingMessageId().toULongLong();
    m_location = m_part->location().toString(true);
    createRetrievalAction()->retrieveMessagePart(m_part->location());
}

FetchMessagesAction::FetchMessagesAction(QObject *parent, const QMailMessageIdList &list):
    ClientServiceAction(parent), m_list(list)
{
    m_actionType = ActionType::Immediate;
    m_serviceActionType = ServiceAction::RetrieveAction;
    m_description = QStringLiteral("Fetching %1 messages").arg(m_list.size());
}

void FetchMessagesAction::process()
{
    createRetrievalAction()->retrieveMessages(m_list, QMailRetrievalAction::RetrievalSpecification::Content);
}

OutboxAction::OutboxAction(QObject *parent, const QMailMessage &msg) : ClientServiceAction(parent), m_msg(msg)
{
    m_actionType = ActionType::Immediate;
    m_serviceActionType = ServiceAction::OutboxAction;
    m_description = QStringLiteral("Storing message in outbox");
}

void OutboxAction::process()
{
    m_msg.setStatus(QMailMessage::Outbox, true);
    if (m_msg.id().isValid()) {
        createStorageAction()->updateMessages(QMailMessageList() << m_msg);
    } else {
        createStorageAction()->addMessages(QMailMessageList() << m_msg);
    }
}

SendPendingMessagesAction::SendPendingMessagesAction(QObject *parent, const QMailAccountId &id) :
    ClientServiceAction(parent), m_id(id)
{
    m_actionType = ActionType::Silent;
    m_serviceActionType = ServiceAction::SendAction;
    m_description = QStringLiteral("Sending pending messages for account: %1").arg(m_id.toULongLong());
}

void SendPendingMessagesAction::process()
{
    QMailTransmitAction *ta = createTransmitAction();
    connect(ta, &QMailTransmitAction::messagesTransmitted, this, &SendPendingMessagesAction::messagesSent);
    connect(ta, &QMailTransmitAction::messagesFailedTransmission, this, &SendPendingMessagesAction::messageSendingFailed);
    ta->transmitMessages(m_id);
}

AccountSyncAction::AccountSyncAction(QObject *parent, const QMailAccountId &id): ClientServiceAction(parent), m_id(id)
{
    m_actionType = ActionType::Immediate;
    m_serviceActionType = ServiceAction::SyncAccountAction;
    m_description = QStringLiteral("Synchronizing account %1").arg(id.toULongLong());
}

void AccountSyncAction::process()
{
    createRetrievalAction()->synchronize(m_id, 20);
}
