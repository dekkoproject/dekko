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
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QLoggingCategory>
#include <QObject>
#include <QPointer>
#include <qmailaccount.h>
#include <qmailaccountconfiguration.h>
#include "AccountConfiguration.h"

Q_DECLARE_LOGGING_CATEGORY(D_ACCOUNT)

/** @ingroup group_accounts */
class Account : public QObject
{
    Q_OBJECT
    // TODO: add last synchronized properties to this as well as the other account properties
    Q_PROPERTY(int id READ id WRITE setId NOTIFY accountChanged)
    Q_PROPERTY(int accountId READ id WRITE setId NOTIFY accountChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY accountChanged)
    Q_PROPERTY(QStringList messageSources READ messageSources NOTIFY accountChanged)
    Q_PROPERTY(QStringList messageSinks READ messageSinks NOTIFY accountChanged)
    Q_PROPERTY(QObject *incoming READ incoming NOTIFY accountChanged)
    Q_PROPERTY(QObject *outgoing READ outgoing NOTIFY accountChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY accountChanged)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)

    Q_ENUMS(SpecialUseFolder)
    Q_ENUMS(Error)

public:
    explicit Account(QObject *parent = 0);

    ~Account() {
        delete m_account;
        delete m_accountConfig;
        m_incoming->deleteLater();
        m_outgoing->deleteLater();
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
    QMailAccountId accountId() const { return m_account->id(); }

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

    /** @short Is this account the preferred sending account **/
    bool isPreferredSender() const { return m_account->status() & QMailAccount::PreferredSender; }

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
    Q_INVOKABLE void setSpecialUseFolder(SpecialUseFolder folder, const QString &path);
    Q_INVOKABLE QString specialUseFolderPath(SpecialUseFolder folder);

    QMailAccount *qMailAccount() const { return m_account; }

    static const QString imapServiceType, popServiceType, qmfStorage, smtpServiceType;
    /** @short used by Accounts.h to indicate the account has been updates
     *
     * This is triggered as a result of QMailStore::accountUpdated
     */
    void emitAccountChanged() {
        emit accountChanged(id());
    }

    bool isValid() const { return m_account->id().isValid(); }

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
    /** @short Save account settings
     *
     * Updates or creates a new account if it doesn't already exist
     */
    bool save();

    QString customValue(const QString &ns, const QString &key);
    void setCustomValue(const QString &ns, const QString &key, const QString &value);
protected:
    QMailAccount *m_account;
    QMailAccountConfiguration *m_accountConfig;
    AccountConfiguration *m_incoming;
    AccountConfiguration *m_outgoing;

    void initialize();

private slots:
    void reload(const QMailAccountIdList &ids);

};

/** @ingroup group_accounts */
class NewAccount : public Account
{
    Q_OBJECT
    AccountConfiguration::ServiceType m_type;
public:
    explicit NewAccount(QObject *parent = 0);

    Q_INVOKABLE void setSourceType(const int &srcType);
};

#endif // ACCOUNT_H
