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
#include "Message.h"
#include <qmailfolder.h>
#include <qmailmessagekey.h>
#include <qmailstore.h>
#include <Account.h>

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
    return QMailMessage(m_id).subject().simplified();
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

bool MinimalMessage::isTodo() const
{
    return (QMailMessageMetaData(m_id).status() & QMailMessageMetaData::Todo);
}

bool MinimalMessage::canBeRestored() const
{
    return QMailMessage(m_id).restoreFolderId().isValid();
}

bool MinimalMessage::isDone() const
{
    QString done = QMailMessage(m_id).customField("task-done");
    if (done.isEmpty()) {
        return false;
    }
    return done.toInt() != 0;
}

bool MinimalMessage::isJunk() const
{
    return (QMailMessageMetaData(m_id).status() & QMailMessageMetaData::Junk);
}

bool MinimalMessage::isListPost() const
{
    return !QMailMessage(m_id).listId().isEmpty();
}

QString MinimalMessage::previousFolderName() const
{
    return QMailFolder(QMailMessage(m_id).restoreFolderId()).displayName();
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

QString MinimalMessage::prettyLongDate()
{
    QDateTime t = date();
    return QStringLiteral("%1, %2").arg(t.toString(tr("dddd dd")), t.toString(tr("hh:mm")));
}

Qt::CheckState MinimalMessage::checked() const
{
    return m_checked;
}

/** @short returns a messagekey suitable for showing a filtered list of messages from this sender */
QVariant MinimalMessage::senderMsgKey() const
{
    QMailMessageKey excludeKey = QMailMessageKey::status((QMailMessage::Removed | QMailMessage::Trash), QMailDataComparator::Excludes);
    return QMailMessageKey::sender(m_from->address(), QMailDataComparator::Includes) & excludeKey;
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
    emit internalMessageChanged();
}

void MinimalMessage::setChecked(const Qt::CheckState &checked)
{
    if (checked == m_checked) {
        return;
    }
    m_checked = checked;
    emit checkedChanged();
}

void MinimalMessage::setIsTodo(const bool todo)
{
    QMailMessageMetaData mmd(m_id);
    mmd.setStatus(QMailMessageMetaData::Todo, todo);
    QMailStore::instance()->updateMessage(&mmd);
    emit minMessageChanged();
}


Message::Message(QObject *parent) : MinimalMessage(parent),
    m_to(0), m_cc(0), m_bcc(0), m_attachments(0), m_preferPlainText(false)
{
    connect(this, &Message::internalMessageChanged, this, &Message::initMessage);
    connect(QMailStore::instance(), SIGNAL(messagesUpdated(QMailMessageIdList)), this, SLOT(handleUpdatedMessages(QMailMessageIdList)));
    m_to = new QQmlObjectListModel<MailAddress>(this);
    m_cc = new QQmlObjectListModel<MailAddress>(this);
    m_bcc = new QQmlObjectListModel<MailAddress>(this);
    m_attachments = new Attachments(this);
}

QString Message::toRecipientsString()
{
    QMailAccountId accId(QMailMessage(m_id).parentAccountId());
    Account *account = new Account(this);
    account->setId(accId.toULongLong());
    AccountConfiguration *config = static_cast<AccountConfiguration *>(account->incoming());

    auto listContainsMe = [](AccountConfiguration *config, const QStringList &recips) -> int {
        int idx = -1;
        for (int i = 0; i < recips.size(); ++i) {
            QString recip = recips.at(i);
            if (config->name() == recip || config->email() == recip) {
                return i;
            }
        }
        return idx;
    };

    if (!config) {
        qDebug() << "Failed to create account configuration";
        return QString("Failed");
    }
    QStringList addr; // list of addresses to compare
    // create our list of TO recipients addresses
    Q_FOREACH(auto a, m_to->toList()) {
        addr.append(a->address());
    }
    int myIndex = listContainsMe(config, addr);
    // we actually want the name here
    addr.clear();
    Q_FOREACH(auto n, m_to->toList()) {
        addr.append(n->name());
    }
    // Now move "Me" to the start
    if (myIndex != -1) {
        addr.move(myIndex, 0);
        // and rename to "you" so that is is displayed
        // like "to you, Bob, Phil..."
        addr.replace(0, tr("you"));
    }
    return tr("to %1").arg(addr.join(", "));
}

QUrl Message::body() const
{
    return m_body;
}

QUrl Message::findInterestingBodyPart(const QMailMessageId &id, const bool preferPlainText)
{
    if (!id.isValid()) {
        return QUrl();
    }
    QMailMessage msg(id);
    bool isPlainText = false;
    QString msgIdString = QString::number(id.toULongLong());
    QString location;
    QUrl url;

    if (msg.multipartType() == QMailMessage::MultipartNone) {
        isPlainText = (msg.body().contentType().content() == QByteArrayLiteral("text/plain"));
        url.setScheme(QStringLiteral("dekko-msg"));
    } else {
        QMailMessagePart *part = 0;
        if (!preferPlainText && msg.hasHtmlBody()) {
            QMailMessagePartContainer *html = msg.findHtmlContainer();
            if (html) {
                part = static_cast<QMailMessagePart *>(html);
            }
        }
        if (!part || preferPlainText) {
            QMailMessagePartContainer *ptext = msg.findPlainTextContainer();
            if (ptext) {
                part = static_cast<QMailMessagePart *>(ptext);
                isPlainText = true;
            }
        }
        if (!part) {
            qDebug() << __func__ << "Unable to find a displayable message part :-/";
            return QUrl();
        }
        location = part->location().toString(true);
        url.setScheme(QStringLiteral("dekko-part"));
    }
    url.setHost(QStringLiteral("msg"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("messageId"), msgIdString);
    if (isPlainText) {
        query.addQueryItem(QStringLiteral("requestFormatting"), QStringLiteral("true"));
    }
    if (!location.isEmpty()) {
        query.addQueryItem(QStringLiteral("location"), location);
    }
    url.setQuery(query);
//    qDebug() << url;
    return url;
}

bool Message::preferPlainText() const
{
    return m_preferPlainText;
}

void Message::setPreferPlainText(const bool preferPlainText)
{
    if (m_preferPlainText == preferPlainText)
        return;

    m_preferPlainText = preferPlainText;
    emit plainTextChanged();
}

void Message::initMessage()
{
    if (m_body.isValid()) {
        return;
    }
    QUrl url = Message::findInterestingBodyPart(m_id, m_preferPlainText);
    if (url.isValid()) {
        m_body = url;
        emit bodyChanged();
    }

    auto appendAddresses = [](QQmlObjectListModel<MailAddress> *model, const QMailAddressList &addresses) {
        Q_FOREACH(auto address, addresses) {
            model->append(new MailAddress(0, address));
        }
    };
    QMailMessage msg(m_id);
    appendAddresses(m_to, msg.to());
    appendAddresses(m_cc, msg.cc());
    appendAddresses(m_bcc, msg.bcc());
    m_attachments->setMessageId(m_id);
    emit messageChanged();
}

void Message::handleUpdatedMessages(const QMailMessageIdList &list)
{
    if (list.contains(m_id)) {
        // Something changed so signal changes
        emit minMessageChanged();
        emit messageChanged();
    }
}
