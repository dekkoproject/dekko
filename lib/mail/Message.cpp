#include "Message.h"

MinimalMessage::MinimalMessage(QObject *parent) : QObject(parent),
    m_from(0), m_checked(Qt::Unchecked)
{
}

MailAddress* MinimalMessage::from() const
{
    return m_from;
}

QString MinimalMessage::subject() const
{
    return QMailMessage(m_id).subject();
}

QString MinimalMessage::preview() const
{
    return QMailMessageMetaData(m_id).preview().simplified();
}

bool MinimalMessage::hasAttachments() const
{
    return (QMailMessageMetaData(m_id).status() & QMailMessageMetaData::HasAttachments);
}

bool MinimalMessage::isRead() const
{
    return (QMailMessageMetaData(m_id).status() & QMailMessageMetaData::Read);
}

bool MinimalMessage::isFlagged() const
{
    return (QMailMessageMetaData(m_id).status() & QMailMessageMetaData::Important);
}

QDateTime MinimalMessage::date() const
{
    return QMailMessageMetaData(m_id).date().toLocalTime();
}

QString MinimalMessage::prettyDate()
{
    QDateTime timestamp = date();
    if (!timestamp.isValid())
        return QString();

    if (timestamp.date() == QDate::currentDate())
        return timestamp.toString(tr("hh:mm"));

    int beforeDays = timestamp.date().daysTo(QDate::currentDate());
    if (beforeDays <= 7) {
        return timestamp.toString(tr("ddd hh:mm"));
    } else {
        return timestamp.toString(tr("dd MMM"));
    }
}

Qt::CheckState MinimalMessage::checked() const
{
    return m_checked;
}

void MinimalMessage::setMessageId(const quint64 &id)
{
    setMessageId(QMailMessageId(id));
}

void MinimalMessage::setMessageId(const QMailMessageId &id)
{
    m_id = id;
    if (!m_id.isValid()) {
        return;
    }
    QMailMessage msg(m_id);
//    qDebug() << "MSG ID: " << messageId();
    m_from = new MailAddress(this);
    m_from->setAddress(msg.from());

    emit minMessageChanged();
}

void MinimalMessage::setChecked(const Qt::CheckState &checked)
{
    if (checked == m_checked) {
        return;
    }
    m_checked = checked;
    emit checkedChanged();
}


Message::Message(QObject *parent) : MinimalMessage(parent),
    m_to(0), m_cc(0), m_bcc(0)
{

}
