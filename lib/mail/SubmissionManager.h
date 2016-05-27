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

signals:
    void builderChanged();
    void error(const Error &error);
    void messageQueued();
    void savingDraftSilently();
    void draftSavedSilently();
    void draftSaved();

public slots:
    void setBuilder(QObject * builder);
    void send();
    void saveDraft(const bool userTriggered = false);
//    void respond(const ResponseType &type);
    void messageSent(const QMailMessageIdList &ids);
    void reset();

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
