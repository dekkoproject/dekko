#include "SubmissionManager.h"
#include <qmailmessage.h>
#include <qmailstore.h>

SubmissionManager::SubmissionManager(QObject *parent) : QObject(parent),
    m_builder(Q_NULLPTR)
{
    connect(Client::instance(), &Client::messagesSent, this, &SubmissionManager::messageSent);

    m_timer.setInterval(30000);
    connect(&m_timer, &QTimer::timeout, [=](){saveDraft(false);});
}

QObject *SubmissionManager::builder() const
{
    return m_builder;
}

bool SubmissionManager::canSend() const
{
    return m_builder->hasRecipients() && !m_builder->subject()->textDocument()->isEmpty();
}

void SubmissionManager::setBuilder(QObject *builder)
{
    MessageBuilder *b = qobject_cast<MessageBuilder *>(builder);
    if (m_builder == b)
        return;

    m_builder = b;
    connect(m_builder, &MessageBuilder::maybeStartSaveTimer, this, &SubmissionManager::maybeStartSaveTimer);
    emit builderChanged();
}

void SubmissionManager::send()
{
    if (!hasBuilder() || !hasIdentities()) {
        return;
    }
    if (!canSend()) {
        emit error(Error::InvalidMessage);
        return;
    }
    m_timer.stop();
    // So we first save it as the *final* draft and then use that draft message as the one to send
    saveDraft(false);
    // Ok so this function doesn't actually send anything. All we
    // are actually doing is grabbing the constructed message, checking if we can reference
    // it externally from the imap server for the lemonade profile stuff and handing it
    // over to the Client service which will manage the storage and submission at the
    // most appropriate time.
    QMailMessage msg(m_builder->lastDraftId());
//    qDebug() << msg.toRfc2822(QMailMessage::TransmissionFormat);
    QMailAccount account(msg.parentAccountId());
    if ((account.status() & QMailAccount::CanReferenceExternalData) &&
            (account.status() & QMailAccount::CanTransmitViaReference) &&
            account.standardFolder(QMailFolder::SentFolder).isValid() &&
            QMailFolder(account.standardFolder(QMailFolder::SentFolder)).id().isValid()) {
        msg.setStatus(QMailMessage::TransmitFromExternal, true);
    }
    Client::instance()->sendMessage(msg);
    emit messageQueued();
}

void SubmissionManager::saveDraft(const bool userTriggered)
{
    if (!hasBuilder() || !hasIdentities()) {
        return;
    }
    if (!userTriggered) {
        emit savingDraftSilently();
    }
    QMailMessage msg = m_builder->message();
    msg.setStatus(QMailMessage::Draft, true);
    msg.setStatus(QMailMessage::Outbox, false);
    bool saved = false;
    if (!msg.id().isValid()) {
        // local only
        msg.setStatus(QMailMessage::LocalOnly, true);
        saved = Client::instance()->addMessage(&msg);
    } else {
        QMailMessageId id = msg.id();
        msg.setId(QMailMessageId());
        msg.setStatus(QMailMessage::LocalOnly, true);
        msg.setServerUid(QString());
        saved = Client::instance()->addMessage(&msg);
        Client::instance()->removeMessage(id, QMailStore::CreateRemovalRecord);
    }
    // now queue an update at some point with the server. It doesn't
    // have to be immediate as we have it localy
    if (saved) {
        Client::instance()->moveToStandardFolder(QMailMessageIdList() << msg.id(), Folder::SpecialUseDraftsFolder);
        if (userTriggered) {
            emit draftSaved();
        } else {
            emit draftSavedSilently();
        }
        m_builder->setLastDraftId(msg.id());
    } else {
        // TODO: emit an error
    }
}

void SubmissionManager::messageSent(const QMailMessageIdList &ids)
{
    qDebug() << ids.count() << "messages sent";
}

void SubmissionManager::reset()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    }
    m_builder->reset();
}

void SubmissionManager::discard()
{
    m_timer.stop();
    QMailMessageId lastId = m_builder->lastDraftId();
    if (lastId.isValid()) {
        Client::instance()->removeMessage(lastId, QMailStore::CreateRemovalRecord);
    }
    reset();
    emit messageDiscarded();
}

bool SubmissionManager::hasBuilder()
{
    if (m_builder == Q_NULLPTR) {
        // requires a constructed message builder
        emit error(Error::NoBuilder);
        return false;
    } else {
        return true;
    }
}

bool SubmissionManager::hasIdentities()
{
    if (!m_builder->hasIdentities()) {
        emit error(Error::NoIdentities);
        return false;
    }
    return true;
}

void SubmissionManager::maybeStartSaveTimer()
{
    if (m_builder->hasRecipients()) {
        // trigger a save 3 seconds from now as to not cause that nasty block
        // when the models are updating
        QTimer::singleShot(3000, this, [=](){ saveDraft(false); } );
        m_timer.start();
    }
}

