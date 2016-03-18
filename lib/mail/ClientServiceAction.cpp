#include "ClientServiceAction.h"
#include <qmaildisconnected.h>
#include <qmailmessage.h>
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
