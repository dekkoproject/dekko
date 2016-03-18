#include "MessageFilterCollection.h"
#include "qmailstore.h"
#include <QTimer>

MessageFilterCollection::MessageFilterCollection(QObject *parent) : QObject(parent), m_children(0),
    m_filter(None), m_firstRun(true)
{
    m_children = new QQmlObjectListModel<MessageSet>(this);
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
    }
}

void MessageFilterCollection::createStandardFolderCollection()
{
    // InboxSet
    QMailMessageKey inboxKey;
    StandardFolderSet *inbox = new StandardFolderSet(); // model will take ownership
    inbox->setType(StandardFolderSet::SpecialUseInboxFolder);
    inbox->init(tr("Inbox"), inboxKey);
    m_children->append(inbox);

    // Drafts
    QMailMessageKey draftsKey;
    draftsKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Draft, QMailDataComparator::Includes));
    draftsKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash | QMailMessage::Outbox, QMailDataComparator::Excludes));
    StandardFolderSet *drafts = new StandardFolderSet();
    drafts->setType(StandardFolderSet::SpecialUseDraftsFolder);
    drafts->initNoDecendents(tr("Drafts"), draftsKey);
    m_children->append(drafts);

    // Junk
    QMailMessageKey junkKey;
    junkKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Junk, QMailDataComparator::Includes));
    junkKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes));
    StandardFolderSet *junk = new StandardFolderSet();
    junk->setType(StandardFolderSet::SpecialUseJunkFolder);
    junk->initNoDecendents(tr("Spam"), junkKey);
    m_children->append(junk);

    // Outbox
    QMailMessageKey outboxKey;
    outboxKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Outbox, QMailDataComparator::Includes));
    outboxKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes));
    StandardFolderSet *outbox = new StandardFolderSet();
    outbox->setType(StandardFolderSet::SpecialUseOutboxFolder);
    outbox->initNoDecendents(tr("Outbox"), outboxKey);
    m_children->append(outbox);

    // Sent
    QMailMessageKey sentKey;
    sentKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Sent, QMailDataComparator::Includes));
    sentKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Excludes));
    StandardFolderSet *sent = new StandardFolderSet();
    sent->setType(StandardFolderSet::SpecialUseSentFolder);
    sent->initNoDecendents(tr("Sent"), sentKey);
    m_children->append(sent);

    // Trash
    QMailMessageKey trashKey;
    trashKey &= QMailMessageKey(QMailMessageKey::status(QMailMessage::Trash, QMailDataComparator::Includes));
    StandardFolderSet *trash = new StandardFolderSet();
    trash->setType(StandardFolderSet::SpecialUseTrashFolder);
    trash->initNoDecendents(tr("Trash"), trashKey);
    m_children->append(trash);
}

