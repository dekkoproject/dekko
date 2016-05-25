#include "SubmissionManager.h"
#include <qmailmessage.h>

SubmissionManager::SubmissionManager(QObject *parent) : QObject(parent),
    m_builder(Q_NULLPTR)
{
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
    // Ok so this function doesn't actually send anything. All we
    // are actually doing is grabbing the constructed message, checking if we can reference
    // it externally from the imap server for the lemonade profile stuff and handing it
    // over to the Client service which will manage the storage and submission at the
    // most appropriate time.
    QMailMessage msg = m_builder->message();
    qDebug() << msg.toRfc2822(QMailMessage::TransmissionFormat);
    QMailAccount account(msg.parentAccountId());
    if ((account.status() & QMailAccount::CanReferenceExternalData) &&
            (account.status() & QMailAccount::CanTransmitViaReference) &&
            account.standardFolder(QMailFolder::SentFolder).isValid() &&
            QMailFolder(account.standardFolder(QMailFolder::SentFolder)).id().isValid()) {
        msg.setStatus(QMailMessage::TransmitFromExternal, true);
    }
    Client::instance()->sendMessage(msg);
}

void SubmissionManager::saveDraft()
{
    if (!hasBuilder() || !hasIdentities()) {
        return;
    }
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

