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
#include "Account.h"
#include <QDebug>
#include <qmailfolder.h>
#include <qmailmessage.h>
#include <qmailserviceconfiguration.h>
#include <qmailstore.h>

Q_LOGGING_CATEGORY(D_ACCOUNT, "dekko.accounts.account")

const QString Account::imapServiceType = QStringLiteral("imap4");
const QString Account::popServiceType = QStringLiteral("pop3");
const QString Account::qmfStorage = QStringLiteral("qmfstoragemanager");
const QString Account::smtpServiceType = QStringLiteral("smtp");

Account::Account(QObject *parent) : QObject(parent),
    m_account(new QMailAccount()),
    m_accountConfig(new QMailAccountConfiguration()),
    m_incoming(Q_NULLPTR),
    m_outgoing(Q_NULLPTR)
{
//    connect(QMailStore::instance(), SIGNAL(accountsUpdated(QMailAccountIdList)), this, SLOT(reload(QMailAccountIdList)));
    m_account->setMessageType(QMailMessage::Email);
    initialize();
}

void Account::setSpecialUseFolder(Account::SpecialUseFolder folder, const quint64 &folderId)
{
    QMailFolderId fId(folderId);
    if (fId.isValid()) {
        m_account->setStandardFolder((QMailFolder::StandardFolder)folder, fId);
    } else {
        qCDebug(D_ACCOUNT) << __func__ << "Invalid folder id";
        emit error(Error::InvalidFolderId, id());
    }
}

void Account::setSpecialUseFolder(Account::SpecialUseFolder folder, const QString &path)
{
    const QMailFolderIdList folders = QMailStore::instance()->queryFolders(
                QMailFolderKey::path(path) & QMailFolderKey::parentAccountId(m_account->id())
                );
    if (path.isEmpty() || folders.count() == 0) {
        m_account->setStandardFolder((QMailFolder::StandardFolder)folder, QMailFolderId());
        return;
    }
    if (folders.count() != 1) {
        return;
    }
    setSpecialUseFolder(folder, folders.first().toULongLong());
}

QString Account::specialUseFolderPath(Account::SpecialUseFolder folder)
{
    QMailFolderId folderId = m_account->standardFolder((QMailFolder::StandardFolder)folder);
    return folderId.isValid() ? QMailFolder(folderId).path() : QString();
}

void Account::setId(const int &id)
{
    QMailAccountId aId(id);
    if (aId.isValid()) {
        m_account = new QMailAccount(aId);
        m_accountConfig = new QMailAccountConfiguration(m_account->id());
        initialize(); // TODO: is this needed?
        emit accountChanged(aId.toULongLong());
    } else {
        qCDebug(D_ACCOUNT)  << __func__ << "Account id: '" << aId.toULongLong() << "' is not valid";
        emit error(Error::InvalidAccountId, id);
    }
}

void Account::setName(const QString &name)
{
    if (!name.isEmpty()) {
        m_account->setName(name);
    } else {
        qCDebug(D_ACCOUNT)  << __func__ << "Empty name";
        emit error(Error::EmptyName, id());
    }
}

bool Account::save()
{
    bool result;
    m_account->setStatus(QMailAccount::UserEditable, true);
    m_account->setStatus(QMailAccount::UserRemovable, true);
    m_account->setStatus(QMailAccount::MessageSource, true);
    m_account->setStatus(QMailAccount::CanRetrieve, true);
    m_account->setStatus(QMailAccount::MessageSink, true);
    m_account->setStatus(QMailAccount::CanTransmit, true);
    m_account->setStatus(QMailAccount::Enabled, true);
    m_account->setStatus(QMailAccount::CanCreateFolders, false);
    m_account->setFromAddress(QMailAddress(m_outgoing->name(), m_outgoing->email()));
    if (m_account->id().isValid()) {
        qCDebug(D_ACCOUNT)  << __func__ << "Updating account settings for: " << m_account->id();
        result = QMailStore::instance()->updateAccount(m_account, m_accountConfig);
    } else {
        qCDebug(D_ACCOUNT)  << __func__ << "Creating new account";
        result = QMailStore::instance()->addAccount(m_account, m_accountConfig);
    }
    if (result) {
        qCDebug(D_ACCOUNT)  << __func__ << "Success";
    } else {
        qCDebug(D_ACCOUNT)  << __func__ << "Oops that didn't work,' something bad happened";
    }
    return result;
}

QString Account::customValue(const QString &ns, const QString &key)
{
    return m_account->customField(QString("%1.%2").arg(ns, key));
}

void Account::setCustomValue(const QString &ns, const QString &key, const QString &value)
{
    m_account->setCustomField(QString("%1.%2").arg(ns, key), value);
    QMailStore::instance()->updateAccount(m_account);
}

void Account::initialize()
{
    QStringList accountServices = m_accountConfig->services();
    // Need to check the qmfstoragemanager is an available service
    if (!accountServices.contains(qmfStorage)) {
//        qCDebug(D_ACCOUNT)  << __func__ << "qmfstoragemanager not in services, adding it now";
        m_accountConfig->addServiceConfiguration(qmfStorage);
        QMailServiceConfiguration qmfStorageConfig(m_accountConfig, qmfStorage);
        qmfStorageConfig.setType(QMailServiceConfiguration::Storage);
        qmfStorageConfig.setVersion(101);
        qmfStorageConfig.setValue(QStringLiteral("basePath"), QStringLiteral(""));
    }
    if (!accountServices.contains(QStringLiteral("smtp"))) {
//        qCDebug(D_ACCOUNT)  << __func__ << "smtp not in services, adding now";
        m_accountConfig->addServiceConfiguration(smtpServiceType);
    }
    QString recvType;
    if (accountServices.contains(imapServiceType)) {
        recvType = imapServiceType;
    } else if (accountServices.contains(popServiceType)) {
        recvType = popServiceType;
    } else {
        // default to imap
        recvType = imapServiceType;
        m_accountConfig->addServiceConfiguration(imapServiceType);
    }

    if (recvType == imapServiceType) {
        m_incoming = new ImapAccountConfiguration(this, m_accountConfig, recvType);
    } else {
        m_incoming = new PopAccountConfiguration(this, m_accountConfig, recvType);
    }
    m_outgoing = new SmtpAccountConfiguration(this, m_accountConfig, smtpServiceType);
}

void Account::reload(const QMailAccountIdList &ids)
{
    if (!ids.contains(m_account->id())) {
        return;
    }
    qCDebug(D_ACCOUNT)  << __func__ << "Reloading account";
    delete m_incoming;
    m_incoming = 0;
    delete m_outgoing;
    m_outgoing = 0;
    delete m_accountConfig;
    m_accountConfig = 0;
    m_accountConfig = new QMailAccountConfiguration();
    QMailAccountId id(m_account->id());
    delete m_account;
    m_account = 0;
    setId(id.toULongLong());
    emit accountChanged(m_account->id().toULongLong());
    m_incoming->emitConfigurationChanges();
    m_outgoing->emitConfigurationChanges();
}

NewAccount::NewAccount(QObject *parent) : Account(parent)
{
}

void NewAccount::setSourceType(const int &srcType)
{
    m_type = (AccountConfiguration::ServiceType)srcType;
    delete m_accountConfig;
    m_accountConfig = 0;
    m_accountConfig = new QMailAccountConfiguration();
    m_account->setStatus(QMailAccount::UserEditable, true);
    m_account->setStatus(QMailAccount::UserRemovable, true);
    QStringList accountServices = m_accountConfig->services();
    if (!accountServices.contains(qmfStorage)) {
        m_accountConfig->addServiceConfiguration(qmfStorage);
        QMailServiceConfiguration qmfStorageConfig(m_accountConfig, qmfStorage);
        qmfStorageConfig.setType(QMailServiceConfiguration::Storage);
        qmfStorageConfig.setVersion(101);
        qmfStorageConfig.setValue(QStringLiteral("basePath"), QStringLiteral(""));
    }
    if (m_type == AccountConfiguration::IMAP) {
        qCDebug(D_ACCOUNT) << "Create new imap configuration";
        if (!accountServices.contains(imapServiceType)) {
            qCDebug(D_ACCOUNT) << "Adding IMAP service configuration";
            m_accountConfig->addServiceConfiguration(imapServiceType);
        }
        m_incoming = new ImapAccountConfiguration(this, m_accountConfig, imapServiceType);
    } else if (m_type == AccountConfiguration::POP3) {
        qCDebug(D_ACCOUNT) << "Create new pop configuration";
        if (!accountServices.contains(popServiceType)) {
            qCDebug(D_ACCOUNT) << "Adding pop service configuration";
            m_accountConfig->addServiceConfiguration(popServiceType);
        }
        m_incoming = new PopAccountConfiguration(this, m_accountConfig, popServiceType);
    }
    if (!accountServices.contains(QStringLiteral("smtp"))) {
        qCDebug(D_ACCOUNT)  << __func__ << "smtp not in services, adding now";
        m_accountConfig->addServiceConfiguration(smtpServiceType);
    }
    m_outgoing = new SmtpAccountConfiguration(this, m_accountConfig, smtpServiceType);
}
