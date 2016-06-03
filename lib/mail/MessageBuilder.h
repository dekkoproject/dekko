#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <QQuickTextDocument>
#include <QStringList>
#include <qmailmessage.h>
#include "Attachments.h"
#include "MailAddress.h"
#include "SenderIdentities.h"


class MessageBuilder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *to READ to NOTIFY modelsChanged)
    Q_PROPERTY(QObject *cc READ cc NOTIFY modelsChanged)
    Q_PROPERTY(QObject *bcc READ bcc NOTIFY modelsChanged)
    Q_PROPERTY(QObject *attachments READ attachments NOTIFY modelsChanged)
    Q_PROPERTY(QQuickTextDocument *subject READ subject WRITE setSubject NOTIFY subjectChanged)
    Q_PROPERTY(QQuickTextDocument *body READ body WRITE setBody NOTIFY bodyChanged)
    Q_PROPERTY(QObject *identities READ identities WRITE setIdentities NOTIFY identitiesChanged)
    Q_PROPERTY(bool hasDraft READ hasDraft CONSTANT)
    Q_ENUMS(RecipientModels)
public:
    explicit MessageBuilder(QObject *parent = Q_NULLPTR);

    enum RecipientModels { To, Cc, Bcc };

    QObject *to() const { return m_to; }
    QObject *cc() const { return m_cc; }
    QObject *bcc() const {return m_bcc; }
    QObject *attachments() const { return m_attachments; }

    QQuickTextDocument *subject() const;
    QQuickTextDocument *body() const;
    QMailMessage message();

    bool hasRecipients() { return !m_to->isEmpty(); }
    bool hasIdentities() { return !m_identities->isEmpty(); }
    QObject *identities() const;
    QMailMessageId lastDraftId() const {return m_lastDraftId;}
    void setLastDraftId(const QMailMessageId &id);

    bool hasDraft() { return m_lastDraftId.isValid(); }

signals:
    void modelsChanged();
    void subjectChanged(QQuickTextDocument *subject);
    void bodyChanged(QQuickTextDocument *body);
    void identitiesChanged();
    void maybeStartSaveTimer();

public slots:
    void addRecipient(const RecipientModels which, const QString &emailAddress);
    void addRecipient(const RecipientModels which, const QString &name, const QString &address);
    void removeRecipient(const RecipientModels which, const int &index);
    void addFileAttachment(const QString &file);
    void addFileAttachments(const QStringList &files);
    void appendTextToBody(const QString &text);
    // TODO: inline/attached messages and messageparts

    void removeAttachment(const int &index);
    void reset();
    void setSubject(QQuickTextDocument *subject);

    void setBody(QQuickTextDocument *body);
    void setIdentities(QObject *identities);

private:
    QQmlObjectListModel<MailAddress> *m_to;
    QQmlObjectListModel<MailAddress> *m_cc;
    QQmlObjectListModel<MailAddress> *m_bcc;
    QQmlObjectListModel<Attachment> *m_attachments;
    QQuickTextDocument *m_subject;
    QQuickTextDocument *m_body;
    SenderIdentities *m_identities;
    quint64 m_sourceStatus;
    QMailMessageId m_lastDraftId;
};

#endif // MESSAGEBUILDER_H
