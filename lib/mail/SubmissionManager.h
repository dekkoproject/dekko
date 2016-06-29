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
#ifndef SUBMISSIONMANAGER_H
#define SUBMISSIONMANAGER_H

#include <QObject>
#include <QTimer>
#include <qmailaccount.h>
#include <qmailmessage.h>
#include "Client.h"
#include "MessageBuilder.h"
#include "SenderIdentities.h"

class SubmissionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *builder READ builder WRITE setBuilder NOTIFY builderChanged)
    Q_PROPERTY(bool canSend READ canSend CONSTANT)
    Q_ENUMS(ResponseType)
    Q_ENUMS(Error)
public:
    explicit SubmissionManager(QObject *parent = 0);

    enum ResponseType {
        None,
        Reply,
        ReplyAll,
        ReplyList,
        ForwardInline,
        ForwardAsAttachment
    };

    enum Error {
        NoBuilder,
        InvalidMessage,
        NoIdentities
    };

    QObject *builder() const;
    bool canSend() const;
    void respondToMessage(const ResponseType &type, const QMailMessageId &msgId);
    void forwardMessage(const ResponseType &type, const QMailMessageId &msgId);

signals:
    void builderChanged();
    void error(const Error &error);
    void messageQueued();
    void savingDraftSilently();
    void draftSavedSilently();
    void draftSaved();
    void messageDiscarded();

public slots:
    void setBuilder(QObject * builder);
    void send();
    void saveDraft(const bool userTriggered = false);
//    void respond(const ResponseType &type);
    void messageSent(const QMailMessageIdList &ids);
    void respondToMessage(const ResponseType &type, const quint64 &msgId);
    void forwardMessage(const ResponseType &type, const quint64 &msgId);
    void reset();
    void discard();

protected:
    bool hasBuilder();
    bool hasIdentities();

private slots:
    void maybeStartSaveTimer();

private:
    MessageBuilder *m_builder;
    QTimer m_timer;
};

#endif // SUBMISSIONMANAGER_H
