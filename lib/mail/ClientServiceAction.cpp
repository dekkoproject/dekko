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

ExportUpdatesAction::ExportUpdatesAction(QObject *parent, QMailRetrievalAction *action, const QMailAccountId &id) :
    ClientServiceAction(parent), m_accountId(id), m_action(action)
{
    m_actionType = ActionType::Silent;
    m_serviceActionType = ServiceAction::ExportAction;
    m_description = tr("Syncing changes for %1 account").arg(QMailAccount(m_accountId).name());
}

void ExportUpdatesAction::process()
{
    qDebug() << "Exporting updates for account: " << QMailAccount(m_accountId).name();
    m_action->exportUpdates(m_accountId);
}
