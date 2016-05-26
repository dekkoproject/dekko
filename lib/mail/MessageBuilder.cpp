#include "MessageBuilder.h"
#include <AccountConfiguration.h>

MessageBuilder::MessageBuilder(QObject *parent) : QObject(parent),
    m_to(Q_NULLPTR),m_cc(Q_NULLPTR), m_bcc(Q_NULLPTR), m_subject(Q_NULLPTR),
    m_body(Q_NULLPTR), m_identities(Q_NULLPTR), m_sourceStatus(QMailMessage::LocalOnly)
{
    m_to = new QQmlObjectListModel<MailAddress>(this);
    m_cc = new QQmlObjectListModel<MailAddress>(this);
    m_bcc = new QQmlObjectListModel<MailAddress>(this);
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
    mail.setBody(QMailMessageBody::fromData(plainTextBody, type, QMailMessageBody::QuotedPrintable));
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

void MessageBuilder::reset()
{
    m_to->clear();
    m_bcc->clear();
    m_cc->clear();
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

