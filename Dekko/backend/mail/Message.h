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
#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QUrl>
#include <qmailmessage.h>
#include <QmlObjectListModel.h>
#include "MailAddress.h"
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include "Attachments.h"

/** @short MinimalMessage provides just the required info for display in the message list

    There isn't need to create the full Message here but instead return it as a property
    of this minimal property set.
*/
class MinimalMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int messageId READ messageId WRITE setMessageId NOTIFY minMessageChanged)
    Q_PROPERTY(int parentAccountId READ parentAccountId NOTIFY minMessageChanged)
    Q_PROPERTY(MailAddress *from READ from NOTIFY minMessageChanged)
    Q_PROPERTY(QString subject READ subject NOTIFY minMessageChanged)
    Q_PROPERTY(QString preview READ preview NOTIFY minMessageChanged)
    Q_PROPERTY(bool hasAttachments READ hasAttachments NOTIFY minMessageChanged)
    Q_PROPERTY(bool isRead READ isRead NOTIFY minMessageChanged)
    Q_PROPERTY(bool isImportant READ isFlagged NOTIFY minMessageChanged)
    Q_PROPERTY(bool isTodo READ isTodo WRITE setIsTodo NOTIFY minMessageChanged)
    Q_PROPERTY(bool isDone READ isDone NOTIFY minMessageChanged)
    Q_PROPERTY(bool isJunk READ isJunk NOTIFY minMessageChanged)
    Q_PROPERTY(bool canBeRestored READ canBeRestored NOTIFY minMessageChanged)
    Q_PROPERTY(bool isListPost READ isListPost NOTIFY minMessageChanged)
    Q_PROPERTY(bool isDraft READ isDraft NOTIFY minMessageChanged)
    Q_PROPERTY(QString previousFolderName READ previousFolderName NOTIFY minMessageChanged)
    Q_PROPERTY(QDateTime date READ date NOTIFY minMessageChanged)
    Q_PROPERTY(QString prettyDate READ prettyDate NOTIFY minMessageChanged)
    Q_PROPERTY(QString prettyLongDate READ prettyLongDate NOTIFY minMessageChanged)
    Q_PROPERTY(Qt::CheckState checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(QVariant senderMsgKey READ senderMsgKey NOTIFY minMessageChanged)

public:
    explicit MinimalMessage(QObject *parent = 0);
    int messageId() const { return m_id.toULongLong(); }
    int parentAccountId() const { return QMailMessageMetaData(m_id).parentAccountId().toULongLong(); }
    MailAddress *from() const;
    QString subject() const;
    QString preview() const;
    bool hasAttachments() const;
    bool isRead() const;
    bool isFlagged() const;
    bool isTodo() const;
    bool canBeRestored() const;
    bool isDone() const;
    bool isJunk() const;
    bool isListPost() const;
    bool isDraft() const;
    QString previousFolderName() const;
    QDateTime date() const;
    QString prettyDate();
    QString prettyLongDate();
    Qt::CheckState checked() const;
    QVariant senderMsgKey() const;

signals:
    void internalMessageChanged();
    void minMessageChanged();
    void checkedChanged();

public slots:
    void setMessageId(const quint64 &id);
    void setMessageId(const QMailMessageId &id);
    void emitMinMessageChanged() { emit minMessageChanged(); }
    void selectionEnded() { setChecked(Qt::Unchecked); }
    void selectionStarted() { setChecked(Qt::Unchecked); }
    void setChecked(const Qt::CheckState &checked);
    void setIsTodo(const bool todo);

protected:
    QMailMessageId m_id;

private:
    MailAddress *m_from;
    Qt::CheckState m_checked;
};

class Message : public MinimalMessage // Extend on what we already have above
{
    Q_OBJECT
    Q_PROPERTY(QObject *to READ to NOTIFY messageChanged)
    Q_PROPERTY(QObject *cc READ cc NOTIFY messageChanged)
    Q_PROPERTY(QObject *bcc READ bcc NOTIFY messageChanged)
    Q_PROPERTY(QUrl body READ body NOTIFY bodyChanged)
    // THis property converts the to model into a readable string
    // e.g "to Dan, Bob, you, ubuntu-phone, ubuntu...."
    Q_PROPERTY(QString toRecipientsString READ toRecipientsString NOTIFY messageChanged)
    Q_PROPERTY(bool preferPlainText READ preferPlainText WRITE setPreferPlainText NOTIFY plainTextChanged)
    Q_PROPERTY(QObject *attachments READ attachments NOTIFY messageChanged)
public:
    explicit Message(QObject *parent = 0);
    QObject *to() const { return m_to; }
    QObject *cc() const { return m_cc; }
    QObject *bcc() const { return m_bcc; }
    QObject *attachments() const { return m_attachments; }
    QString toRecipientsString();

    QUrl body() const;
    static QUrl findInterestingBodyPart(const QMailMessageId &id, const bool preferPlainText);

    bool preferPlainText() const;

public slots:
    void setPreferPlainText(const bool preferPlainText);

signals:
    void messageChanged();
    void bodyChanged();

    void plainTextChanged();

private slots:
    void initMessage();
    void handleUpdatedMessages(const QMailMessageIdList &list);

private:
    QQmlObjectListModel<MailAddress> *m_to;
    QQmlObjectListModel<MailAddress> *m_cc;
    QQmlObjectListModel<MailAddress> *m_bcc;
    Attachments *m_attachments;
    QUrl m_body;
    bool m_preferPlainText;
};

#endif // MESSAGE_H
