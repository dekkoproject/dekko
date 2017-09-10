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
#include "MessageSet.h"
#include <QDebug>
#include <qmailstore.h>

MessageSet::MessageSet(QObject *parent) : QObject(parent), m_children(0)
{
    m_children = new QQmlObjectListModel<MessageSet>(this);
    connect(m_children, &QQmlObjectListModel<MessageSet>::countChanged,
            this, &MessageSet::descendentsCountChanged);
    connect(QMailStore::instance(),
            SIGNAL(folderContentsModified(const QMailFolderIdList&)),
            this,
            SIGNAL(countChanged()));
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

void StandardFolderSet::trackAccountChanges()
{
    connect(QMailStore::instance(), &QMailStore::accountsAdded, this, &StandardFolderSet::accountsAdded);
    connect(QMailStore::instance(), &QMailStore::accountsRemoved, this, &StandardFolderSet::accountsRemove);
    connect(QMailStore::instance(), &QMailStore::accountsUpdated, this, &StandardFolderSet::accountsChanged);
}

void StandardFolderSet::accountsAdded(const QMailAccountIdList &idList)
{
    if (queryEnabledAccounts().isEmpty()) {
        return;
    }
    Q_FOREACH(auto &id, idList) {
        if (!m_inboxList.contains(id)) {
            auto set = new StandardFolderSet();
            set->setType(SpecialUseInboxFolder);
            set->initNoDecendents(QMailAccount(id).name(), createAccountDescendentKey(id, QMailFolder::InboxFolder));
            m_children->append(set);
            m_inboxList.append(id);
        } else {
            qDebug() << "[StandardFolderSet]" << __func__ << "Account with same id already a descendent";
        }
    }
}

void StandardFolderSet::accountsRemove(const QMailAccountIdList &idList)
{
    Q_FOREACH(auto id, idList) {
        int index = m_inboxList.indexOf(id);
        if (index == -1) {
            continue;
        }
        m_children->remove(index);
        m_inboxList.removeAt(index);
    }
}

void StandardFolderSet::accountsChanged(const QMailAccountIdList &idList)
{
    auto enabledIds = queryEnabledAccounts();

    Q_FOREACH(auto &id, idList) {
        if (!enabledIds.contains(id)) {
            int index = m_inboxList.indexOf(id);
            if (index == -1)
                continue;
            m_children->remove(index);
            m_inboxList.removeAt(index);
        }
    }

    Q_FOREACH(auto &enabledId, enabledIds) {
        int index = m_inboxList.indexOf(enabledId);
        if (index == -1) {
            auto set = new StandardFolderSet();
            set->setType(SpecialUseInboxFolder);
            set->initNoDecendents(QMailAccount(enabledId).name(), createAccountDescendentKey(enabledId, QMailFolder::InboxFolder));
            m_children->append(set);
            m_inboxList.append(enabledId);
        }
    }
}

void StandardFolderSet::appendInboxDescendents()
{
    Q_FOREACH(const QMailAccountId &id, queryEnabledAccounts()) {
        auto set = new StandardFolderSet();
        set->setType(SpecialUseInboxFolder);
        set->initNoDecendents(QMailAccount(id).name(), createAccountDescendentKey(id, QMailFolder::InboxFolder));
        m_children->append(set);
        m_inboxList.append(id);
    }
}

QMailMessageKey StandardFolderSet::createAccountDescendentKey(const QMailAccountId &id, const QMailFolder::StandardFolder &folderType)
{
    QMailAccount account(id);
    QMailFolderId folderId = account.standardFolder(folderType);
    QMailFolderKey inboxKey = QMailFolderKey::id(folderId, QMailDataComparator::Equal);
    QMailMessageKey excludeRemovedKey = QMailMessageKey::status(QMailMessage::Removed,  QMailDataComparator::Excludes);
    QMailMessageKey msgKey = QMailMessageKey::parentFolderId(inboxKey) & excludeRemovedKey;
    if (msgKey.isEmpty()) {
        return QMailMessageKey::nonMatchingKey();
    }
    return msgKey;
}

QMailAccountIdList StandardFolderSet::queryEnabledAccounts()
{
    return QMailStore::instance()->queryAccounts(QMailAccountKey::messageType(QMailMessage::Email)
                                                 & QMailAccountKey::status(QMailAccount::Enabled),
                                                 QMailAccountSortKey::name());
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
    {
        appendInboxDescendents();
        trackAccountChanges();
        break;
    }
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

QVariant StandardFolderSet::descendentsKey()
{
    QMailFolderIdList descendentFolders;
    Q_FOREACH(const QMailAccountId &id, queryEnabledAccounts()) {
        QMailAccount account(id);
        QMailFolderId folderId;
        if (m_type == SpecialUseInboxFolder) {
            folderId = account.standardFolder(QMailFolder::InboxFolder);
            if (folderId.isValid()) {
                descendentFolders << account.standardFolder(QMailFolder::InboxFolder);
            }
        }
    }
    QMailMessageKey excludeRemovedKey = QMailMessageKey::status(QMailMessage::Removed,  QMailDataComparator::Excludes);
    QMailFolderKey inboxKey = QMailFolderKey::id(descendentFolders, QMailDataComparator::Includes);
    QMailMessageKey messageKey =  QMailMessageKey::parentFolderId(inboxKey) & excludeRemovedKey;
    return messageKey;
}

SmartFolderSet::SmartFolderSet(QObject *parent) : MessageSet(parent),
    m_type(SmartTodoFolder), m_timer(0)
{
    connect(this, &SmartFolderSet::countChanged, this, &SmartFolderSet::smartCountChanged);
}

void SmartFolderSet::init(const QString &displayName, const QMailMessageKey &messageKey)
{
    m_name = displayName;
    m_key = messageKey;
    emit displayNameChanged();
    emit messageKeyChanged();

    if (m_type == SmartTodayFolder) {
        if (!m_timer) {
            m_timer = new QTimer(this);
            m_timer->setInterval(10000);
            connect(m_timer, &QTimer::timeout, this, &SmartFolderSet::updateDescription);
            m_timer->start();
        }
    }
}

SmartFolderSet::SmartFolderType SmartFolderSet::type() const
{
    return m_type;
}

void SmartFolderSet::setType(SmartFolderSet::SmartFolderType type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged(type);
}

void SmartFolderSet::updateDescription()
{
    m_name = tr("Today, %1").arg(QDateTime::currentDateTime().toString("ddd d"));
    emit displayNameChanged();
}

int SmartFolderSet::smartCount()
{
    if (m_type == SmartFolderSet::SmartTodoFolder) {
        return totalCount();
    } else {
        return unreadCount();
    }
}
