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
#include "MessageFilterCollection.h"
#include "qmailstore.h"
#include <QTimer>
#include <QDateTime>

MessageFilterCollection::MessageFilterCollection(QObject *parent) : QObject(parent), m_children(0),
    m_filter(None), m_firstRun(true)
{
    m_children = new QQmlObjectListModel<MessageSet>(this);
    connect(QMailStore::instance(),
            SIGNAL(accountsAdded(QMailAccountIdList)),
            this,
            SLOT(reset()));
    connect(QMailStore::instance(),
            SIGNAL(accountsRemoved(QMailAccountIdList)),
            this,
            SLOT(reset()));
}

void MessageFilterCollection::setFilter(MessageFilterCollection::Filter filter)
{
    if (m_filter == filter)
        return;

    m_filter = filter;
    emit filterChanged(filter);
    if (m_firstRun) {
        QTimer::singleShot(350, this, SLOT(reset()));
        m_firstRun = false;
    } else {
        reset();
    }
}

void MessageFilterCollection::reset()
{
    if (!m_children->isEmpty()) {
        m_children->clear();
    }
    switch(m_filter) {
    case None:
        return;
    case StandardFolders:
    {
        createStandardFolderCollection();
        return;
    }
    case SmartFolders:
    {
        createSmartFolderCollection();
        return;
    }
    }
}

void MessageFilterCollection::createStandardFolderCollection()
{
    // InboxSet
    QMailMessageKey inboxKey;
    StandardFolderSet *inbox = new StandardFolderSet(); // model will take ownership
    inbox->setType(StandardFolderSet::SpecialUseInboxFolder);
    inbox->init(tr("All inboxes"), inboxKey);
    inbox->setSupportedActions(MessageSet::SyncAction | MessageSet::MarkAsRead | MessageSet::Expunge |  MessageSet::SearchFolder | MessageSet::ShowProperties);
    m_children->append(inbox);

    // Drafts
    QMailMessageKey draftsKey;
    draftsKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Draft, QMailDataComparator::Includes));
    draftsKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash | QMailMessage::Outbox, QMailDataComparator::Excludes));
    StandardFolderSet *drafts = new StandardFolderSet();
    drafts->setType(StandardFolderSet::SpecialUseDraftsFolder);
    drafts->initNoDecendents(tr("Drafts"), draftsKey);
    drafts->setSupportedActions(MessageSet::ShowProperties | MessageSet::SearchFolder);
    m_children->append(drafts);

    // Junk
    QMailMessageKey junkKey;
    junkKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Junk, QMailDataComparator::Includes));
    junkKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes));
    StandardFolderSet *junk = new StandardFolderSet();
    junk->setType(StandardFolderSet::SpecialUseJunkFolder);
    junk->initNoDecendents(tr("Spam"), junkKey);
    junk->setSupportedActions(MessageSet::SyncAction | MessageSet::MarkAsRead | MessageSet::SearchFolder | MessageSet::ShowProperties);
    m_children->append(junk);

    // Outbox
    QMailMessageKey outboxKey;
    outboxKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Outbox, QMailDataComparator::Includes));
    outboxKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes));
    StandardFolderSet *outbox = new StandardFolderSet();
    outbox->setType(StandardFolderSet::SpecialUseOutboxFolder);
    outbox->initNoDecendents(tr("Outbox"), outboxKey);
    outbox->setSupportedActions(MessageSet::MarkAsRead | MessageSet::ExportPending | MessageSet::SearchFolder | MessageSet::ShowProperties);
    m_children->append(outbox);

    // Sent
    QMailMessageKey sentKey;
    sentKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Sent, QMailDataComparator::Includes));
    sentKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes));
    StandardFolderSet *sent = new StandardFolderSet();
    sent->setType(StandardFolderSet::SpecialUseSentFolder);
    sent->initNoDecendents(tr("Sent"), sentKey);
    sent->setSupportedActions(MessageSet::SyncAction | MessageSet::MarkAsRead | MessageSet::SearchFolder | MessageSet::ShowProperties);
    m_children->append(sent);

    // Trash
    QMailMessageKey trashKey;
    trashKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Includes));
    StandardFolderSet *trash = new StandardFolderSet();
    trash->setType(StandardFolderSet::SpecialUseTrashFolder);
    trash->initNoDecendents(tr("Trash"), trashKey);
    trash->setSupportedActions(MessageSet::SyncAction | MessageSet::MarkAsRead | MessageSet::SearchFolder | MessageSet::ShowProperties | MessageSet::EmptyTrash);
    m_children->append(trash);
}

void MessageFilterCollection::createSmartFolderCollection()
{
    QMailMessageKey todayKey;
    todayKey &= QMailMessageKey(QMailMessageKey::receptionTimeStamp(QDateTime(QDate::currentDate()), QMailDataComparator::GreaterThanEqual));
    todayKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Removed, QMailDataComparator::Excludes));
    SmartFolderSet *todaySet = new SmartFolderSet();
    todaySet->setType(SmartFolderSet::SmartTodayFolder);
    todaySet->init(tr("Today, %1").arg(QDateTime::currentDateTime().toString("ddd d")), todayKey);
    todaySet->setSupportedActions(MessageSet::MarkAsRead | MessageSet::SearchFolder | MessageSet::ShowProperties);
    m_children->append(todaySet);

    QMailMessageKey todoKey;
    todoKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Todo, QMailDataComparator::Includes));
    todoKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Removed, QMailDataComparator::Excludes));
    SmartFolderSet *todoSet = new SmartFolderSet();
    todoSet->setType(SmartFolderSet::SmartTodoFolder);
    todoSet->init(tr("To-do"), todoKey);
    todoSet->setSupportedActions(MessageSet::SearchFolder | MessageSet::ShowProperties | MessageSet::MarkAsDone);
    m_children->append(todoSet);

    QMailMessageKey doneKey;
    doneKey &= QMailMessageKey(QMailMessageKey::customField(QStringLiteral("task-done"), QStringLiteral("1"), QMailDataComparator::Includes));
    doneKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Removed, QMailDataComparator::Excludes));
    SmartFolderSet *doneSet = new SmartFolderSet();
    doneSet->setType(SmartFolderSet::SmartDoneFolder);
    doneSet->init(tr("Done"), doneKey);
    doneSet->setSupportedActions(MessageSet::SearchFolder | MessageSet::ShowProperties);
    m_children->append(doneSet);
}

