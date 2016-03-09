#include "Account.h"
#include <QDebug>
#include <qmailfolder.h>
#include <qmailmessage.h>
#include <qmailserviceconfiguration.h>
#include <qmailstore.h>

const QString Account::imapServiceType = QStringLiteral("imap4");
const QString Account::popServiceType = QStringLiteral("pop3");
const QString Account::qmfStorage = QStringLiteral("qmfstoragemanager");
const QString Account::smtpServiceType = QStringLiteral("smtp");

Account::Account(QObject *parent) : QObject(parent),
    m_account(new QMailAccount()),
    m_accountConfig(new QMailAccountConfiguration()),
    m_incoming(0),
    m_outgoing(0)
{
    m_account->setMessageType(QMailMessage::Email);
    initialize();
}

void Account::setSpecialUseFolder(Account::SpecialUseFolder folder, const quint64 &folderId)
{
    QMailFolderId fId(folderId);
    if (fId.isValid()) {
        m_account->setStandardFolder((QMailFolder::StandardFolder)folder, fId);
    } else {
        qDebug() << "[Account]" << __func__ << "Invalid folder id";
        emit error(Error::InvalidFolderId, id());
    }
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
        qDebug() << "[Account]" << __func__ << "Account id: '" << aId.toULongLong() << "' is not valid";
        emit error(Error::InvalidAccountId, m_account->id().toULongLong());
    }
}

void Account::setName(const QString &name)
{
    if (!name.isEmpty()) {
        m_account->setName(name);
    } else {
        qDebug() << "[Account]" << __func__ << "Empty name";
        emit error(Error::EmptyName, id());
    }
}

void Account::initialize()
{
    QStringList accountServices = m_accountConfig->services();
    // Need to check the qmfstoragemanager is an available service
    if (!accountServices.contains(qmfStorage)) {
        m_accountConfig->addServiceConfiguration(qmfStorage);
        QMailServiceConfiguration qmfStorageConfig(m_accountConfig, qmfStorage);
        qmfStorageConfig.setType(QMailServiceConfiguration::Storage);
        qmfStorageConfig.setVersion(101);
        qmfStorageConfig.setValue(QStringLiteral("basePath"), QStringLiteral(""));
    }
    if (!accountServices.contains(QStringLiteral("smtp"))) {
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
