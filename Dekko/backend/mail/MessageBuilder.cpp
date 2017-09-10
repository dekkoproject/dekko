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
#include "MessageBuilder.h"
#include <AccountConfiguration.h>
#include <QFileInfo>
#include <QDebug>
#include <QMimeType>
#include <QMimeDatabase>
#include <QStringBuilder>
#include <QUrlQuery>
#include <emailvalidator.h>
#include <Formatting.h>
#include <PolicyManager.h>
#include <SettingsPolicies.h>

MessageBuilder::MessageBuilder(QObject *parent) : QObject(parent),
    m_to(Q_NULLPTR),m_cc(Q_NULLPTR), m_bcc(Q_NULLPTR), m_attachments(Q_NULLPTR), m_subject(Q_NULLPTR), m_internalSubject(Q_NULLPTR),
    m_body(Q_NULLPTR), m_internalBody(Q_NULLPTR), m_identities(Q_NULLPTR), m_sourceStatus(QMailMessage::LocalOnly), m_mode(Mode::New),
    m_replyType(ReplyType::Reply)
{
    m_to = new QQmlObjectListModel<MailAddress>(this);
    m_cc = new QQmlObjectListModel<MailAddress>(this);
    m_bcc = new QQmlObjectListModel<MailAddress>(this);
    m_attachments = new QQmlObjectListModel<Attachment>(this);
    m_internalSubject = new QTextDocument(this);
    m_internalBody = new QTextDocument(this);
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

    IdentityWrapper *w = static_cast<IdentityWrapper *>(m_identities->selectedAccount());
    Account *sender = static_cast<Account *>(w->get_account());
    Identity *identity = static_cast<Identity *>(w->get_identity());
    mail.setParentAccountId(sender->accountId());
    mail.setDate(QMailTimeStamp::currentDateTime());
    mail.setFrom(identity->fromAddress());
    // If there is a replyTo for the identity add that now
    if (!identity->get_replyTo().isEmpty()) {
        mail.setReplyTo(QMailAddress(identity->get_name(), identity->get_replyTo()));
    }

    auto createAddressList = [](const QQmlObjectListModel<MailAddress> *model) -> QList<QMailAddress> {
        QList<QMailAddress> addrList;
        foreach(MailAddress *addr, model->toList()) {
            addrList << addr->qMailAddress();
        }
        return addrList;
    };

    // append cc/bcc includes
//    MailPolicy *policy = PolicyManager::instance()->mailPolicy(sender->accountId().toULongLong());
//    if (policy) {
//        QStringList ccInc = policy->ccIncludes().split(",");
//        if (!ccInc.isEmpty()) {
//            for( const QString &cc : ccInc) {
//                if (EmailValidator::instance()->validate(cc.trimmed())) {
//                    addRecipient(Cc, cc.trimmed());
//                }
//            }
//        }
//        QStringList bccInc = policy->bccIncludes().split(",");
//        if (!bccInc.isEmpty()) {
//            for( const QString &bcc : ccInc) {
//                if (EmailValidator::instance()->validate(bcc.trimmed())) {
//                    addRecipient(Bcc, bcc.trimmed());
//                }
//            }
//        }
//    }
    mail.setTo(createAddressList(m_to));
    if (!m_cc->isEmpty()) {
        mail.setCc(createAddressList(m_cc));
    }
    if (!m_bcc->isEmpty()) {
        mail.setBcc(createAddressList(m_bcc));
    }
    mail.setSubject(m_internalSubject->toPlainText());
    QString plainTextBody = m_internalBody->toPlainText();
    plainTextBody.append(QStringLiteral("\n\n-- \n%1").arg(identity->get_signature()));
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

    switch(m_mode) {
    case New:
    {
        mail.setResponseType(QMailMessage::NoResponse);
        break;
    }
    case Rply:
    {
        switch(m_replyType) {
        case Reply:
        case ReplyList:
        {
            mail.setResponseType(QMailMessage::Reply);
            break;
        }
        case ReplyAll:
        {
            mail.setResponseType(QMailMessage::ReplyToAll);
            break;
        }
        }
        break;
    }
    case Fwd:
    {
        mail.setResponseType(QMailMessage::Forward);
        break;
    }
    }

    if (mail.responseType() == QMailMessage::Reply || mail.responseType() == QMailMessage::ReplyToAll) {
        if (m_srcMessageId.isValid()) {
            mail.setInResponseTo(m_srcMessageId);
            QMailMessage srcMsg(m_srcMessageId);
            QString refs = srcMsg.headerFieldText(QStringLiteral("References"));
            if (refs.isEmpty()) {
                // use In-Reply-To
                refs = srcMsg.headerFieldText(QStringLiteral("In-Reply-To"));
            }
            QString precursorId(srcMsg.headerFieldText("Message-ID"));
            if (!precursorId.isEmpty()) {
                mail.setInReplyTo(precursorId);

                if (!refs.isEmpty()) {
                    refs.append(' ');
                }
                refs.append(precursorId);
            }
            if (!refs.isEmpty()) {
                mail.setHeaderField("References", refs);
            }
        }
    }
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

void MessageBuilder::buildResponse(const MessageBuilder::ReplyType &type, const QMailMessage &src)
{
    if (!hasIdentities()) {
        qFatal("No identities");
        return;
    }
    // next select the identity to use
    int identityIdx = m_identities->determinePreferredIdentity(src);
    m_identities->setSelectedIndex(identityIdx);
    // next build recipient lists
    buildRecipientsLists(type, src);
    // subject
    const QString subject = Formatting::mangleReplySubject(src.subject().simplified());
    if (m_subject == Q_NULLPTR) {
        m_internalSubject->setPlainText(subject);
    } else {
        m_subject->textDocument()->setPlainText(subject);
    }

    // body
    QString body;
    if (src.multipartType() == QMailMessagePartContainer::MultipartNone) {
        if (src.hasBody()) {
            if (src.hasHtmlBody()) {
                qDebug() << "IsHtml";
                QTextDocument b;
                b.setHtml(src.body().data());
                body = b.toPlainText();
            } else {
                qDebug() << "IsPlaintTExt";
                body = src.body().data();
            }
        }
    } else {
        // were a multipart
        QMailMessagePart *part = Q_NULLPTR;
        // Atm we prefer plaintext parts until we have an proper html editor
        QMailMessagePartContainer *ptext = src.findPlainTextContainer();
        if (ptext) {
            // YAY!
            part = static_cast<QMailMessagePart *>(ptext);
            body = part->body().data();
        } else {
            QMailMessagePartContainer *htext = src.findHtmlContainer();
            if (htext) {
                part = static_cast<QMailMessagePart *>(htext);
                QTextDocument b;
                b.setHtml(part->body().data());
                body = b.toPlainText();
            }
        }
    }
    QString replyHeader = tr("On %1, %2 wrote:\n").arg(src.date().toString(), src.from().name());
    QString quotedBody = replyHeader + QStringList(Formatting::quoteBody(body.split("\n"))).join(QStringLiteral("\n"));

    if (m_body == Q_NULLPTR) {
        m_internalBody->setPlainText(quotedBody);
    } else {
        m_body->textDocument()->setPlainText(quotedBody);
    }
    // attachments ?? not for a reply!!!
    m_mode = Mode::Rply;
    m_replyType = type;
    m_srcMessageId = src.id();
}

void MessageBuilder::buildForward(const MessageBuilder::ForwardType &type, const QMailMessage &src)
{
    Q_UNUSED(type);
    const QString subject = Formatting::mangleForwardSubject(src.subject().simplified());
    if (m_subject == Q_NULLPTR) {
        m_internalSubject->setPlainText(subject);
    } else {
        m_subject->textDocument()->setPlainText(subject);
    }
    // So what we have to think about is....
    QString body;
    QString srcBody;
    bool hasInlineBody(false);
    // if message only has a plaintext part then we forward it inline
    if (src.hasBody() && src.hasPlainTextBody()) {
        //use it
        hasInlineBody = true;
        srcBody = src.body().data();
        // if it is just a multipart/alternative with no other attachments then inline the text/plain
    } else if (src.multipartType() == QMailMessage::MultipartAlternative) {
        QMailMessagePartContainer *ptext = src.findPlainTextContainer();
        if (ptext) {
            // use it
            hasInlineBody = true;
            srcBody = static_cast<QMailMessagePart *>(ptext)->body().data();
            // FIXME: fetch the plaintext part!!!
            if (srcBody.isEmpty() && src.hasHtmlBody()) {
                QMailMessagePartContainer *htext = src.findHtmlContainer();
                if (htext) {
                    QMailMessagePart *part = static_cast<QMailMessagePart *>(htext);
                    QTextDocument b;
                    b.setHtml(part->body().data());
                    srcBody = b.toPlainText();
                }
            }
        }
    }

    if (hasInlineBody) {
        QString forwardBlock = "\n------------ " + tr("Forwarded Message") + " ------------\n";
        forwardBlock += tr("Date: ") + src.date().toString() + '\n';
        forwardBlock += tr("From: ") + src.from().toString() + '\n';
        forwardBlock += tr("To: ") + QMailAddress::toStringList(src.to()).join(QLatin1String(", ")) + '\n';
        forwardBlock += tr("Subject: ") + src.subject().simplified() + '\n';
        body = (forwardBlock % QStringLiteral("\n") % srcBody);
    } else {
        // We need to attach the message instead using part refs if possible
        Attachment *srcAttach = new Attachment(this, QString::number(src.id().toULongLong()), Attachment::PartType::Message, Attachment::Disposition::Attached);
        m_attachments->append(srcAttach);
    }
    if (m_body == Q_NULLPTR) {
        m_internalBody->setPlainText(body);
    } else {
        m_body->textDocument()->setPlainText(body);
    }
    m_mode = Mode::Fwd;
    m_srcMessageId = src.id();
}

void MessageBuilder::composeMailTo(const QString &mailtoUri)
{
    QUrl mailto(mailtoUri);
    if (mailto.isEmpty() || mailto.scheme() != QStringLiteral("mailto")) {
        qDebug() << "Not a valid mailto url" << mailto;
        return;
    }
    QString host; // Will be our "To" addresses that aren't part of the queries
    QString tos; // query to=
    QString ccs; // query cc=
    QString bccs; // query bcc=
    QString encoded = mailto.path(QUrl::FullyDecoded).replace(QStringLiteral("mailto:"), QStringLiteral(""));
//    qDebug() << "Encoded path" << encoded;
    int index = encoded.indexOf(QChar('?'));
    if (index != -1) {
        QString query = encoded.mid(index + 1);
//        qDebug() << "Query: " << query;
        QUrlQuery opts(query);
        if (opts.hasQueryItem(QStringLiteral("subject"))) {
            setSubjectText(opts.queryItemValue(QStringLiteral("subject")));
        }
        if (opts.hasQueryItem(QStringLiteral("body"))) {
            setBodyText(opts.queryItemValue(QStringLiteral("body")));
        }
        if (opts.hasQueryItem(QStringLiteral("to"))) {
            tos = opts.queryItemValue(QStringLiteral("to"));
        }
        if (opts.hasQueryItem(QStringLiteral("cc"))) {
            ccs = opts.queryItemValue(QStringLiteral("cc"));
        }
        if (opts.hasQueryItem(QStringLiteral("bcc"))) {
            bccs = opts.queryItemValue(QStringLiteral("bcc"));
        }
        host = encoded.left(index);
    } else {
        host = encoded;
    }
    auto parseRecipients = [=](const QString &val) -> QStringList {
        QStringList result;
        if (val.contains(QChar(';'))) {
            Q_FOREACH(const QString &addr, val.split(QChar(';'))) {
                QMailAddress address(addr);
                if (EmailValidator::instance()->validate(address.address())) {
                    result << addr;
                } else {
                    qDebug() << "Invalid mailto address" << addr << ". Ignoring!!!!";
                }
            }
        } else if (val.contains(QChar(','))) {
            Q_FOREACH(const QString &addr, val.split(QChar(','))) {
                QMailAddress address(addr);
                if (EmailValidator::instance()->validate(address.address())) {
                    result << addr;
                } else {
                    qDebug() << "Invalid mailto address" << addr << ". Ignoring!!!!";
                }
            }
        } else {
            QMailAddress address(val);
            if (EmailValidator::instance()->validate(address.address())) {
                result << val;
            } else {
                qDebug() << "Invalid mailto address" << val << ". Ignoring!!!!";
            }
        }
        int ok = result.removeDuplicates();
        Q_UNUSED(ok);
        return result;
    };
    // need to make sure we include the tos as it maybe something stupid like
    // mailto:test1@example.com?to=test2@example.com,test3@example.com&cc=blah@meh.org...
    if (!tos.isEmpty()) {
        host.append(QChar(','));
        host.append(tos);
    }
    QStringList tosList = parseRecipients(host);
    Q_FOREACH(const QString &addr, tosList) {
        addRecipient(To, addr);
    }
    if (!ccs.isEmpty()) {
        QStringList ccList = parseRecipients(ccs);
        Q_FOREACH(const QString &addr, ccList) {
            addRecipient(Cc, addr);
        }
    }
    if (!bccs.isEmpty()) {
        QStringList bccList = parseRecipients(bccs);
        Q_FOREACH(const QString &addr, bccList) {
            addRecipient(Bcc, addr);
        }
    }
}

void MessageBuilder::reloadLastDraftId()
{
    QMailMessage src(m_lastDraftId);
    m_identities->setSelectedIndex(m_identities->determinePreferredIdentity(src));
    if (src.responseType() == QMailMessage::Reply || src.responseType() == QMailMessage::ReplyToAll) {
        m_srcMessageId = src.inResponseTo();
        m_mode = Mode::Rply;
    } else if (src.responseType() == QMailMessage::Reply) {
        m_mode = Mode::Fwd;
    } else {
        m_mode = Mode::New;
    }
    addRecipients(To, src.to());
    addRecipients(Cc, src.cc());
    addRecipients(Bcc, src.bcc());
    setSubjectText(src.subject());
    if (src.multipartType() == QMailMessagePartContainer::MultipartNone) {
        if (src.hasBody()) {
            setBodyText(src.body().data());
        }
    } else {
        // were a multipart
        QMailMessagePart *part = Q_NULLPTR;
        // Atm we prefer plaintext parts until we have an proper html editor
        QMailMessagePartContainer *ptext = src.findPlainTextContainer();
        if (ptext) {
            // YAY!
            part = static_cast<QMailMessagePart *>(ptext);
            setBodyText(part->body().data());
        } else {
            QMailMessagePartContainer *htext = src.findHtmlContainer();
            if (htext) {
                part = static_cast<QMailMessagePart *>(htext);
                QTextDocument b;
                b.setHtml(part->body().data());
                setBodyText(b.toPlainText());
            }
        }
    }

    // TODO: attachments
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

void MessageBuilder::addRecipients(const MessageBuilder::RecipientModels which, const QMailAddressList &addresses)
{
    if (addresses.isEmpty()) {
        return;
    }
    Q_FOREACH(const QMailAddress & addr, addresses) {
        switch(which) {
        case To:
            m_to->append(new MailAddress(Q_NULLPTR, addr));
            break;
        case Cc:
            m_cc->append(new MailAddress(Q_NULLPTR, addr));
            break;
        case Bcc:
            m_bcc->append(new MailAddress(Q_NULLPTR, addr));
            break;
        }
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

void MessageBuilder::addFileAttachment(const QString &file)
{
    if (file.isEmpty()) {
        return;
    }
    QString newFile;
    if (file.startsWith(QStringLiteral("file://"))) {
        qDebug() << "Stripping file://";
        newFile = file.split(QStringLiteral("://")).at(1);
    } else {
        newFile = file;
    }
    if (QFile::exists(newFile)) {
        addFileAttachments(QStringList() << newFile);
    } else {
        qDebug() << "Attachment url " << newFile << "doesn't exist on the local file system";
    }
}

void MessageBuilder::addFileAttachments(const QStringList &files)
{
    Q_FOREACH(const QString &file, files) {
        Attachment *a = new Attachment(this, file, Attachment::File, Attachment::Attached);
        qDebug() << "======== New File Attachment ========";
        qDebug() << "DisplayName: " << a->displayName();
        qDebug() << "Size: " << a->size();
        qDebug() << "Mimetype: " << a->mimeType();
        qDebug() << "====================================";
        m_attachments->append(a);
    }
}

void MessageBuilder::appendTextToSubject(const QString &text)
{
    if (m_subject) {
        m_subject->textDocument()->setPlainText(text);
    } else {
        m_internalSubject->setPlainText(text);
    }
}

void MessageBuilder::appendTextToBody(const QString &text)
{
    QString body;
    if (m_body == Q_NULLPTR) {
        body = m_internalBody->toPlainText();
    } else {
        body = m_body->textDocument()->toPlainText();
    }
    if (body.isEmpty()) {
        body.append(text);
    } else {
        body.append(QStringLiteral("\n\n%1").arg(text));
    }
    if (m_body) {
        m_body->textDocument()->setPlainText(body);
    } else {
        m_internalBody->setPlainText(body);
    }
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
        disconnect(m_subject->textDocument(), SIGNAL(contentsChange(int,int,int)), this, SLOT(subjectChanged(int,int,int)));
        m_subject->textDocument()->clear();
    }
    m_subject = Q_NULLPTR;
    m_internalSubject->clear();
    if (m_body != Q_NULLPTR) {
        disconnect(m_body->textDocument(), SIGNAL(contentsChange(int,int,int)), this, SLOT(bodyChanged(int,int,int)));
        m_body->textDocument()->clear();
    }
    m_identities->reset();
    m_body = Q_NULLPTR;
    m_internalBody->clear();
    m_mode = Mode::New;
    m_srcMessageId = QMailMessageId();
}

void MessageBuilder::setSubject(QQuickTextDocument *subject)
{
    if (m_subject == subject)
        return;

    m_subject = subject;
    if (!m_internalSubject->isEmpty()) {
        m_subject->textDocument()->setPlainText(m_internalSubject->toPlainText());
    }
    connect(m_subject->textDocument(), SIGNAL(contentsChange(int,int,int)), this, SLOT(subjectChanged(int,int,int)));
    emit subjectChanged(subject);
}

void MessageBuilder::setBody(QQuickTextDocument *body)
{
    if (m_body == body)
        return;

    m_body = body;
    if (!m_internalBody->isEmpty()) {
        m_body->textDocument()->setPlainText(m_internalBody->toPlainText());
    }
    connect(m_body->textDocument(), SIGNAL(contentsChange(int,int,int)), this, SLOT(bodyChanged(int,int,int)));
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

void MessageBuilder::buildRecipientsLists(const MessageBuilder::ReplyType &type, const QMailMessage &src)
{
    // builds an address list from the To and Reply-To fields
    auto prepareToReplies = [=](const QMailMessage &msg)->QMailAddressList {
        bool hasReplyTo = false;
        QMailAddressList tos;
        // Start with the reply-to field and cater for ML munging
        // we should try and respect the reply-to field if it's valid
        QMailAddress replyTo(msg.replyTo());
        if (replyTo.isNull()) {
            tos << src.from();
        } else if (replyTo.isGroup()) {
            // Check for nasty ML reply-to munging. Yuk!!!
            Q_FOREACH(const QMailAddress &addr, replyTo.groupMembers()) {
                if (EmailValidator::instance()->validate(addr.address())) {
                    hasReplyTo = true;
                    tos << addr; // this one's ok.
                }
            }
        } else {
            if (EmailValidator::instance()->validate(replyTo.address())) {
                hasReplyTo = true;
                tos << replyTo;
            }
        }
        if (hasReplyTo && (type == ReplyAll)) {
            // we don't need to worry about duplicates as they get pruned next;
            tos << src.from();
        }
        if (type == ReplyAll) {
            tos << src.to();
        }
        qDebug() << "TOS COUNT; " << tos.count();
        // if we still have nothing from the reply-to's then just use from
        if (tos.isEmpty()) {
            tos << src.from();
        }
        return tos;
    };

    auto deduplicateAndRemoveMe = [=](const QMailAddressList &list) -> QMailAddressList {
        QMailAddressList tmpList;
        tmpList << list;
        for (int i = 0; i < list.size(); ++i) {
            const QMailAddress addr = list.at(i);
            if (m_identities->hasAddress(addr.address())) {
                tmpList.removeAt(i);
            }
        }
        // now deduplicate
        QMailAddressList result;
        Q_FOREACH(const QMailAddress &addr, tmpList) {
            if (!result.contains(addr)) {
                result << addr;
            }
        }
        return result;
    };

    switch (type) {
    case Reply:
    {
        QMailAddressList t1 = prepareToReplies(src);
        addRecipients(To, deduplicateAndRemoveMe(t1));
        break;
    }
    case ReplyAll:
    {
        QMailAddressList t2 = prepareToReplies(src);

        if (!src.listId().isEmpty()) {
            // include ML address
            QByteArray lp = getListPostAddress(src);
            if (!lp.isEmpty()) {
                t2.append(QMailAddress(QString(lp)));
            }
        }
        addRecipients(To, deduplicateAndRemoveMe(t2));

        QMailAddressList c1 = src.cc();
        if (!c1.isEmpty()) {
            addRecipients(Cc, deduplicateAndRemoveMe(c1));
        }
        QMailAddressList b1 = src.bcc();
        if (!b1.isEmpty()) {
            addRecipients(Bcc, deduplicateAndRemoveMe(b1));
        }
        break;
    }
    case ReplyList:
    {
        QByteArray listPost = getListPostAddress(src);
        if (!listPost.isEmpty()) {
            m_to->append(new MailAddress(Q_NULLPTR, QString(listPost)));
        } else {
            qWarning() << "Invalid List-Post content " + listPost + " Falling back to using From field";
            buildRecipientsLists(Reply, src);
        }
        break;
    }
    }
}

void MessageBuilder::subjectChanged(int position, int charsRemoved, int charsAdded) {
    Q_UNUSED(position);
    Q_UNUSED(charsRemoved);
    Q_UNUSED(charsAdded);
    const QString &subject = m_subject->textDocument()->toPlainText();
    if (subject != m_internalSubject->toPlainText()) {
        m_internalSubject->setPlainText(subject);
    }
}

void MessageBuilder::bodyChanged(int position, int charsRemoved, int charsAdded) {
    Q_UNUSED(position);
    Q_UNUSED(charsRemoved);
    Q_UNUSED(charsAdded);
    const QString &body = m_body->textDocument()->toPlainText();
    if (body != m_internalBody->toPlainText()) {
        m_internalBody->setPlainText(body);
    }
}

QByteArray MessageBuilder::getListPostAddress(const QMailMessage &src)
{
    QMailMessageHeaderField h = src.headerField(QStringLiteral("List-Post"));
    if (h.isNull()) {
        return QByteArray();
    }
    QByteArray listPost = h.content().replace(QByteArrayLiteral("<"), QByteArrayLiteral("")).replace(QByteArrayLiteral(">"), QByteArrayLiteral(""));
    if (EmailValidator::instance()->validate(listPost)) {
        return listPost;
    }
    return QByteArray();
}

void MessageBuilder::setSubjectText(const QString &text)
{
    if (m_subject == Q_NULLPTR) {
        m_internalSubject->setPlainText(text);
    } else {
        m_subject->textDocument()->setPlainText(text);
    }
}

void MessageBuilder::setBodyText(const QString &body)
{
    if (m_body == Q_NULLPTR) {
        m_internalBody->setPlainText(body);
    } else {
        m_body->textDocument()->setPlainText(body);
    }
}
