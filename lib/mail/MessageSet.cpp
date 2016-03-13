#include "MessageSet.h"
#include <qmailstore.h>

MessageSet::MessageSet(QObject *parent) : QObject(parent), m_children(0)
{
    m_children = new QQmlObjectListModel<MessageSet>(this);
    connect(m_children, &QQmlObjectListModel<MessageSet>::countChanged,
            this, &MessageSet::descendentsCountChanged);
}

QVariant MessageSet::descendentsKey() const
{
    QMailMessageKey key;
    foreach(MessageSet *set, m_children->toList()) {
        key |= set->messageKey().value<QMailMessageKey>();
        key |= set->descendentsKey().value<QMailMessageKey>();
    }
    return key;
}

int MessageSet::unreadCount()
{

    if (hasDecendents()) {
        return QMailStore::instance()->countMessages(
                descendentsKey().value<QMailMessageKey>() &
                QMailMessageKey::status(QMailMessage::Read, QMailDataComparator::Excludes)
                );
    } else {
        return QMailStore::instance()->countMessages(
                m_key &
                QMailMessageKey::status(QMailMessage::Read, QMailDataComparator::Excludes)
                );
    }
}

int MessageSet::totalCount()
{
    if (hasDecendents()) {
        return QMailStore::instance()->countMessages(descendentsKey().value<QMailMessageKey>());
    } else {
        return QMailStore::instance()->countMessages(m_key);
    }
}

void MessageSet::setMessageKey(const QMailMessageKey &key)
{
    if (m_key == key)
        return;
    m_key = key;
    emit messageKeyChanged();
}

void MessageSet::setDisplayName(const QString &displayName)
{
    if (m_name == displayName)
        return;
    m_name = displayName;
    emit displayNameChanged();
}


StandardFolderSet::StandardFolderSet(QObject *parent) : MessageSet(parent),
    m_type(StandardFolder)
{
}

void StandardFolderSet::setType(StandardFolderSet::FolderType type)
{
    if (m_type == type)
        return;
    m_type = type;
    emit standardFolderChanged();
}

void StandardFolderSet::appendInboxDescendents()
{
    QMailMessageKey excludeRemovedKey = QMailMessageKey::status(QMailMessage::Removed,  QMailDataComparator::Excludes);
    auto idList = QMailStore::instance()->queryAccounts(QMailAccountKey::status(QMailAccount::Enabled | QMailAccount::MessageSource) ,QMailAccountSortKey());
    Q_FOREACH(const QMailAccountId &id, idList) {
        QMailAccount account(id);
        QMailFolderId folderId = account.standardFolder(QMailFolder::InboxFolder);
        QMailFolderKey inboxKey = QMailFolderKey::id(folderId, QMailDataComparator::Equal);
        QMailMessageKey msgKey = QMailMessageKey::parentFolderId(inboxKey) & excludeRemovedKey;
//        Q_UNUSED(msgKey);
        auto set = new StandardFolderSet();
        set->setType(SpecialUseInboxFolder);
        set->initNoDecendents(account.name(), msgKey);
        m_children->append(set);
    }
}

void StandardFolderSet::init(const QString &displayName, const QMailMessageKey &messageKey)
{
    m_name = displayName;
    m_key = messageKey;
    emit displayNameChanged();
    emit messageKeyChanged();

    switch(m_type) {
    case StandardFolder:
        break;
    case SpecialUseInboxFolder:
        appendInboxDescendents();
        break;
    case SpecialUseDraftsFolder:
    case SpecialUseJunkFolder:
    case SpecialUseOutboxFolder:
    case SpecialUseSentFolder:
    case SpecialUseTrashFolder:
        break;
    }
}

void StandardFolderSet::initNoDecendents(const QString &displayName, const QMailMessageKey &messageKey)
{
    m_name = displayName;
    m_key = messageKey;
    emit displayNameChanged();
    emit messageKeyChanged();
}
