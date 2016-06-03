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
#include "MessageBuilder.h"
#include <AccountConfiguration.h>
#include <QFileInfo>
#include <QMimeType>
#include <QMimeDatabase>

MessageBuilder::MessageBuilder(QObject *parent) : QObject(parent),
    m_to(Q_NULLPTR),m_cc(Q_NULLPTR), m_bcc(Q_NULLPTR), m_attachments(Q_NULLPTR), m_subject(Q_NULLPTR),
    m_body(Q_NULLPTR), m_identities(Q_NULLPTR), m_sourceStatus(QMailMessage::LocalOnly)
{
    m_to = new QQmlObjectListModel<MailAddress>(this);
    m_cc = new QQmlObjectListModel<MailAddress>(this);
    m_bcc = new QQmlObjectListModel<MailAddress>(this);
    m_attachments = new QQmlObjectListModel<Attachment>(this);
    emit modelsChanged();
}

QQuickTextDocument *MessageBuilder::subject() const
{
    return m_subject;
}

QQuickTextDocument *MessageBuilder::body() const
{
    return m_body;
}

QMailMessage MessageBuilder::message()
{
    QMailMessage mail;
    // TODO:
    // http://code.qt.io/cgit/qt-labs/messagingframework.git/tree/examples/qtmail/emailcomposer.cpp#n752

    if (m_lastDraftId.isValid()) {
        mail.setId(m_lastDraftId);
    }

    Account *sender = static_cast<Account *>(m_identities->selectedAccount());
    mail.setParentAccountId(sender->accountId());
    mail.setDate(QMailTimeStamp::currentDateTime());
    mail.setFrom(sender->qMailAccount()->fromAddress());

    auto createAddressList = [](const QQmlObjectListModel<MailAddress> *model) -> QList<QMailAddress> {
        QList<QMailAddress> addrList;
        foreach(MailAddress *addr, model->toList()) {
            addrList << addr->qMailAddress();
        }
        return addrList;
    };

    mail.setTo(createAddressList(m_to));
    if (!m_cc->isEmpty()) {
        mail.setCc(createAddressList(m_cc));
    }
    if (!m_bcc->isEmpty()) {
        mail.setBcc(createAddressList(m_bcc));
    }
    mail.setSubject(m_subject->textDocument()->toPlainText());

    QString plainTextBody = m_body->textDocument()->toPlainText();
    QMailMessageContentType type(QByteArrayLiteral("text/plain; charset=UTF-8"));
    // TODO: Do we want to always encode as QuotedPrintable it's efficient for ASCII text but becomes inefficient
    // for non-ascii chars i.e QChar::unicode() > 127. SHould we iterate over all chars and decide based on that as QString
    // doesn't provide an isAscii??
    if (m_attachments->isEmpty()) {
        mail.setBody(QMailMessageBody::fromData(plainTextBody, type, QMailMessageBody::QuotedPrintable));
    } else {
        QMailMessagePart bodyPart;
        bodyPart.setBody(QMailMessageBody::fromData(plainTextBody, type, QMailMessageBody::QuotedPrintable));
        mail.setMultipartType(QMailMessagePartContainer::MultipartMixed);
        mail.appendPart(bodyPart);
        foreach(Attachment *attachment, m_attachments->toList()) {
            attachment->addToMessage(mail);
        }
    }
    mail.setMessageType(QMailMessage::Email);

    mail.setSize(mail.indicativeSize() * 1024);
    mail.setStatus(QMailMessage::HasAttachments, false);

    if (m_sourceStatus & QMailMessage::LocalOnly) {
        mail.setStatus(QMailMessage::LocalOnly, true);
    }
    // Initial local storage location
    mail.setParentFolderId(QMailFolder::LocalStorageFolderId);
    // mark it as outgoing
    mail.setStatus(QMailMessage::Outgoing, true);
    // mark it as read
//    mail.setStatus(QMailMessage::Read, true); hmm we actually want to show the count in the nav list
    // In case we want to edit it or view it again inform the store
    // that all the content is available
    mail.setStatus(QMailMessage::ContentAvailable, true);
    mail.setStatus(QMailMessage::PartialContentAvailable, true);
    return mail;
}

QObject *MessageBuilder::identities() const
{
    return m_identities;
}

void MessageBuilder::setLastDraftId(const QMailMessageId &id)
{
    if (id.isValid()) {
        m_lastDraftId = id;
    }
}

void MessageBuilder::addRecipient(const MessageBuilder::RecipientModels which, const QString &emailAddress)
{
    if (emailAddress.isEmpty()) {
        return;
    }
    switch(which) {
    case To:
        m_to->append(new MailAddress(Q_NULLPTR, emailAddress));
        break;
    case Cc:
        m_cc->append(new MailAddress(Q_NULLPTR, emailAddress));
        break;
    case Bcc:
        m_bcc->append(new MailAddress(Q_NULLPTR, emailAddress));
        break;
    }
    emit maybeStartSaveTimer();
}

void MessageBuilder::addRecipient(const MessageBuilder::RecipientModels which, const QString &name, const QString &address)
{
    if (address.isEmpty()) {
        return;
    }

    if (name.isEmpty()) {
        // ok only address given. Why are you calling this then?????
        addRecipient(which, address);
        return;
    }

    switch(which) {
    case To:
        m_to->append(new MailAddress(Q_NULLPTR, name, address));
        break;
    case Cc:
        m_cc->append(new MailAddress(Q_NULLPTR, name, address));
        break;
    case Bcc:
        m_bcc->append(new MailAddress(Q_NULLPTR, name, address));
        break;
    }
    emit maybeStartSaveTimer();
}

void MessageBuilder::removeRecipient(const MessageBuilder::RecipientModels which, const int &index)
{
    if (index < 0) {
        return;
    }
    switch(which) {
    case To:
        m_to->remove(index);
        break;
    case Cc:
        m_cc->remove(index);
        break;
    case Bcc:
        m_bcc->remove(index);
        break;
    }
}

void MessageBuilder::addFileAttachment(const QString &file)
{
    if (file.isEmpty()) {
        return;
    }
    if (QFile::exists(file)) {
        addFileAttachments(QStringList() << file);
    } else {
        qDebug() << "Attachment url " << file << "doesn't exist on the local file system";
    }
}

void MessageBuilder::addFileAttachments(const QStringList &files)
{
    Q_FOREACH(const QString &file, files) {
        Attachment *a = new Attachment(this, file, Attachment::File, Attachment::Attached);
        qDebug() << "DisplayName: " << a->displayName();
        qDebug() << "Size: " << a->size();
        qDebug() << "Mimetype: " << a->mimeType();
        m_attachments->append(a);
    }
}

void MessageBuilder::appendTextToBody(const QString &text)
{
    if (m_body == Q_NULLPTR) {
        return;
    }
    QString body = m_body->textDocument()->toPlainText();
    if (body.isEmpty()) {
        body.append(text);
    } else {
        body.append(QStringLiteral("\n\n%1").arg(text));
    }
    m_body->textDocument()->setPlainText(body);
}

void MessageBuilder::removeAttachment(const int &index)
{
    if (index < 0 || index > m_attachments->size()) {
        return;
    }
    m_attachments->remove(index);
}

void MessageBuilder::reset()
{
    m_lastDraftId = QMailMessageId();
    m_to->clear();
    m_bcc->clear();
    m_cc->clear();
    m_attachments->clear();
    if (m_subject != Q_NULLPTR) {
        m_subject->textDocument()->clear();
    }
    if (m_body != Q_NULLPTR) {
        m_body->textDocument()->clear();
    }
}

void MessageBuilder::setSubject(QQuickTextDocument *subject)
{
    if (m_subject == subject)
        return;

    m_subject = subject;
    emit subjectChanged(subject);
}

void MessageBuilder::setBody(QQuickTextDocument *body)
{
    if (m_body == body)
        return;

    m_body = body;
    emit bodyChanged(body);
}

void MessageBuilder::setIdentities(QObject *identities)
{
    SenderIdentities *i = qobject_cast<SenderIdentities *>(identities);
    if (m_identities == i)
        return;

    m_identities = i;
    emit identitiesChanged();
}
