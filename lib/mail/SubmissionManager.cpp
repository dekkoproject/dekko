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
        qDebug() << "No builder or identity";
        return;
    }
    if (!canSend()) {
        qDebug() << "Can't send yet, invalid message";
        emit error(Error::InvalidMessage);
        return;
    }
    qDebug() << "Stopping timer";
    m_timer.stop();
    // So we first save it as the *final* draft and then use that draft message as the one to send
    qDebug() << "Saving final draft";
    saveDraft(false);

    qDebug() << "Final draft id valid? " << m_builder->lastDraftId().isValid();
    // Ok so this function doesn't actually send anything. All we
    // are actually doing is grabbing the constructed message, checking if we can reference
    // it externally from the imap server for the lemonade profile stuff and handing it
    // over to the Client service which will manage the storage and submission at the
    // most appropriate time.
    qDebug() << "Constructing message";
    QMailMessage msg(m_builder->lastDraftId());
    qDebug() << "Msg valid> " << msg.id().isValid();
//    qDebug() << msg.toRfc2822(QMailMessage::TransmissionFormat);
    QMailAccount account(msg.parentAccountId());
    if ((account.status() & QMailAccount::CanReferenceExternalData) &&
            (account.status() & QMailAccount::CanTransmitViaReference) &&
            account.standardFolder(QMailFolder::SentFolder).isValid() &&
            QMailFolder(account.standardFolder(QMailFolder::SentFolder)).id().isValid()) {
        qDebug() << "Enabling transmit from external";
        msg.setStatus(QMailMessage::TransmitFromExternal, true);
    }
    // we also need to update the Replied RepliedToAll and Forwarded status masks if appropriate
    // TODO: move this to after the message has been sent. i.e on SubmissionManager::messageSent() signal
    if (msg.inResponseTo().isValid()) {
        QMailMessageId inReplyTo(msg.inResponseTo());
        if (inReplyTo.isValid()) {
            QMailMessage src(inReplyTo);
            if (msg.responseType() == QMailMessage::Forward) {
                Client::instance()->markMessageForwarded(QMailMessageIdList() << src.id());
            } else {
                Client::instance()->markMessagesReplied(
                            QMailMessageIdList() << src.id(),
                            (msg.responseType() == QMailMessage::ReplyToAll));
            }
        }
    }
    qDebug() << "Queuing message to send";
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
        qDebug() << "LOCAL ONLY";
        msg.setStatus(QMailMessage::LocalOnly, true);
        saved = Client::instance()->addMessage(&msg);
    } else {
        qDebug() << "UPDATING DRAFT";
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
        Client::instance()->moveToStandardFolder(QMailMessageIdList() << msg.id(), Folder::SpecialUseDraftsFolder, false);
        if (userTriggered) {
            emit draftSaved();
        } else {
            emit draftSavedSilently();
        }
        qDebug() << "SETTING LAST ID: " << msg.id();
        m_builder->setLastDraftId(msg.id());
    } else {
        // TODO: emit an error
    }
}

void SubmissionManager::messageSent(const QMailMessageIdList &ids)
{
    qDebug() << ids.count() << "messages sent";
}

void SubmissionManager::respondToMessage(const SubmissionManager::ResponseType &type, const quint64 &msgId)
{
    respondToMessage(type, QMailMessageId(msgId));
}

void SubmissionManager::respondToMessage(const SubmissionManager::ResponseType &type, const QMailMessageId &msgId)
{
    if (!hasBuilder()) {
        qWarning() << "Builder not ready";
        return;
    }
    if (!msgId.isValid()) {
        qWarning() << "Invalid msg id";
        return;
    }

    MessageBuilder::ReplyType rt;
    switch(type) {
    case Reply:
        rt = MessageBuilder::ReplyType::Reply;
        break;
    case ReplyAll:
        rt = MessageBuilder::ReplyType::ReplyAll;
        break;
    case ReplyList:
        rt = MessageBuilder::ReplyType::ReplyList;
        break;
    default:
        qWarning() << "Unexpected responsetype.";
        Q_ASSERT(false);
        return;
    }
    m_builder->buildResponse(rt, QMailMessage(msgId));
    // Normally we would trigger this from the UI side once the first recipient has been added
    // but replies are generated on the c++ side so trigger it here.
    maybeStartSaveTimer();
}

void SubmissionManager::forwardMessage(const SubmissionManager::ResponseType &type, const QMailMessageId &msgId)
{
    if (!hasBuilder()) {
        qWarning() << "Builder not ready";
        return;
    }
    if (!msgId.isValid()) {
        qWarning() << "Invalid msg id";
        return;
    }
    MessageBuilder::ForwardType ft;
    switch(type) {
    case ForwardInline:
        ft = MessageBuilder::ForwardType::Inline;
        break;
    case ForwardAsAttachment:
        ft = MessageBuilder::ForwardType::Attach;
        break;
    default:
        qWarning() << "Unexpected forward type.";
        Q_ASSERT(false);
        return;
    }
    m_builder->buildForward(ft, QMailMessage(msgId));
}

void SubmissionManager::reloadDraft(const quint64 &msgId)
{
    reloadDraft(QMailMessageId(msgId));
}

void SubmissionManager::reloadDraft(const QMailMessageId &msgId)
{
    if (!hasBuilder()) {
        qWarning() << "Builder not ready";
        return;
    }
    if (!msgId.isValid() || !(QMailMessage(msgId).status() & QMailMessage::Draft)) {
        qWarning() << "THis isn't a draft message. Aborting!";
        return;
    }
    m_builder->setLastDraftId(msgId);
    m_builder->reloadLastDraftId();
}

void SubmissionManager::forwardMessage(const SubmissionManager::ResponseType &type, const quint64 &msgId)
{
    forwardMessage(type, QMailMessageId(msgId));
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

