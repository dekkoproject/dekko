#include "MessageBuilder.h"

MessageBuilder::MessageBuilder(QObject *parent) : QObject(parent)
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

void MessageBuilder::addRecipient(const MessageBuilder::RecipientModels which, const QString &emailAddress)
{
    if (emailAddress.isEmpty()) {
        return;
    }
    switch(which) {
    case To:
        m_to->append(new MailAddress(0, emailAddress));
        break;
    case Cc:
        m_cc->append(new MailAddress(0, emailAddress));
        break;
    case Bcc:
        m_bcc->append(new MailAddress(0, emailAddress));
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
        m_to->append(new MailAddress(0, name, address));
        break;
    case Cc:
        m_cc->append(new MailAddress(0, name, address));
        break;
    case Bcc:
        m_bcc->append(new MailAddress(0, name, address));
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
}

void MessageBuilder::setSubject(QQuickTextDocument *subject)
{
    if (m_subject == subject)
        return;

    m_subject = subject;
    emit subjectChanged(subject);
}

