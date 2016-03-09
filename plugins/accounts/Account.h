#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QPointer>
#include <qmailaccount.h>
#include <qmailaccountconfiguration.h>
#include "AccountConfiguration.h"

class Account : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id WRITE setId NOTIFY accountChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY accountChanged)
    Q_PROPERTY(QStringList messageSources READ messageSources NOTIFY accountChanged)
    Q_PROPERTY(QStringList messageSinks READ messageSinks NOTIFY accountChanged)
    Q_PROPERTY(QObject *incoming READ incoming NOTIFY accountChanged)
    Q_PROPERTY(QObject *outgoing READ outgoing NOTIFY accountChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY accountChanged)

    Q_ENUMS(SpecialUseFolder)
    Q_ENUMS(Error)

public:
    explicit Account(QObject *parent = 0);

    ~Account() {
        delete m_account;
        delete m_accountConfig;
        delete m_incoming;
        delete m_outgoing;
    }

    /** @short Maps to QMailFolder::StandardFolder */
    enum SpecialUseFolder {
        InboxFolder = 1,
        OutboxFolder,
        DraftsFolder,
        SentFolder,
        TrashFolder,
        JunkFolder
    };
    /** @short Error reasons */
    enum Error {
        EmptyName,
        InvalidAccountId,
        InvalidFolderId
    };

    /** @short The account's id */
    int id() const { return m_account->id().toULongLong(); }

    /** @short The accounts name/description
     *
     *  Note: This isn't the same as a service configuration name (i.e username)
     */
    QString name() const { return m_account->name(); }

    /** @short List of protocol plugins for retrieiving mail */
    QStringList messageSources() const { return m_account->messageSources(); }

    /** @short List of protocol plugins for sending mail */
    QStringList messageSinks() const { return m_account->messageSinks(); }

    /** @short Incoming message service configuration */
    QObject *incoming() { return m_incoming; }

    /** @short Outgoing message service configuration */
    QObject *outgoing() { return m_outgoing; }

    /** @short Is this account enabled and can be used */
    bool enabled() const { return m_account->status() & QMailAccount::Enabled; }

    /** @short Get QMailFolderId for SpecialUseFolder */
    Q_INVOKABLE quint64 specialUseFolder(SpecialUseFolder folder)
    {
        return m_account->standardFolder((QMailFolder::StandardFolder)folder).toULongLong();
    }

    /** @short Map a QMailFolderId to SpecialUseFolder
     *
     *  emits InvalidFolderId on error
     */
    Q_INVOKABLE void setSpecialUseFolder(SpecialUseFolder folder, const quint64 &folderId);

    static const QString imapServiceType, popServiceType, qmfStorage, smtpServiceType;

signals:
    void accountChanged(const int &id);
    void error(Error error, const int &accountId);

public slots:
    /** @short Set the account's id
     *
     * emits InvalidAccountId on invalid id
     */
    void setId(const int &id);
    /** @short Set the account's name/description
     *
     * emits EmptyName on error
     */
    void setName(const QString &name);

private:
    QMailAccount *m_account;
    QMailAccountConfiguration *m_accountConfig;
    AccountConfiguration *m_incoming;
    AccountConfiguration *m_outgoing;

    void initialize();

};

#endif // ACCOUNT_H
