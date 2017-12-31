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
#include "AccountConfiguration.h"
#include <QStringList>

Q_LOGGING_CATEGORY(D_ACCOUNT_CONF, "dekko.accounts.config")

//TODO: Move to seperate file??
const QString AccountKeys::name = QStringLiteral("username");
const QString AccountKeys::email = QStringLiteral("address");
const QString AccountKeys::password = QStringLiteral("password");
const QString AccountKeys::smtppassword = QStringLiteral("smtppassword");
const QString AccountKeys::server = QStringLiteral("server");
const QString AccountKeys::port = QStringLiteral("port");
const QString AccountKeys::encryption = QStringLiteral("encryption");
const QString AccountKeys::acceptUntrustedCert = QStringLiteral("acceptUntrustedCertificates");
const QString AccountKeys::canDeleteMail = QStringLiteral("canDelete");
const QString AccountKeys::downloadAttachments = QStringLiteral("downloadAttachments");
const QString AccountKeys::autoDownload = QStringLiteral("autoDownload");
const QString AccountKeys::maxMailSize = QStringLiteral("maxSize");
const QString AccountKeys::prefferedSubType = QStringLiteral("textSubtype");
const QString AccountKeys::idleEnabled = QStringLiteral("pushEnabled");
const QString AccountKeys::canIDLE = QStringLiteral("pushCapable");
const QString AccountKeys::baseFolder = QStringLiteral("baseFolder");
const QString AccountKeys::pushFolders = QStringLiteral("pushFolders");
const QString AccountKeys::checkInterval = QStringLiteral("checkInterval");
const QString AccountKeys::checkWhenRoaming = QStringLiteral("intervalCheckRoamingEnabled");
const QString AccountKeys::capabilities = QStringLiteral("capabilities");
const QString AccountKeys::timeTillLogout = QStringLiteral("timeTillLogout");
const QString AccountKeys::searchLimit = QStringLiteral("searchLimit");
const QString AccountKeys::saslMechanism = QStringLiteral("authentication");
const QString AccountKeys::username = QStringLiteral("smtpusername");
const QString AccountKeys::authFromCaps = QStringLiteral("authFromCapabilities");
const QString AccountKeys::signature = QStringLiteral("signature");

namespace {
    QString defaultPort(AccountConfiguration::ServiceType service, AccountConfiguration::EncryptionMethod method) {
        switch(service) {
        case AccountConfiguration::IMAP:
            switch(method) {
            case AccountConfiguration::None:
            case AccountConfiguration::STARTTLS:
                return QStringLiteral("143");
            case AccountConfiguration::SSL:
                return QStringLiteral("993");
            }
        case AccountConfiguration::POP3:
            switch(method) {
            case AccountConfiguration::None:
            case AccountConfiguration::STARTTLS:
                return QStringLiteral("110");
            case AccountConfiguration::SSL:
                return QStringLiteral("995");
            }
        case AccountConfiguration::SMTP:
            switch(method) {
            case AccountConfiguration::None:
            case AccountConfiguration::STARTTLS:
                return QStringLiteral("587");
            case AccountConfiguration::SSL:
                return QStringLiteral("465");
            }
        }
        Q_UNREACHABLE();
    }
}

AccountConfiguration::AccountConfiguration(QObject *parent) : QObject(parent), m_config(0), m_service(0)
{

}

AccountConfiguration::AccountConfiguration(
        QObject *parent,
        QMailAccountConfiguration *config,
        const QString &serviceType) : QObject(parent),
    m_config(0), m_service(0)
{
    init(config, serviceType);
}

AccountConfiguration::~AccountConfiguration()
{
    delete m_service;
}

QString AccountConfiguration::name() const {
    return m_service->value(AccountKeys::name);
}

QString AccountConfiguration::email() const
{
    return m_service->value(AccountKeys::email);
}

QString AccountConfiguration::password() const
{
    if (serviceType() == SMTP) {
        return PasswordHelper::decodePassword(m_service->value(AccountKeys::smtppassword));
    } else {
        return PasswordHelper::decodePassword(m_service->value(AccountKeys::password));
    }
}

QString AccountConfiguration::server() const
{
    return m_service->value(AccountKeys::server);
}

QString AccountConfiguration::port() const
{
    return m_service->value(AccountKeys::port, defaultPort(serviceType(), encryption()));
}

AccountConfiguration::EncryptionMethod AccountConfiguration::encryption() const
{
    return (EncryptionMethod)m_service->value(AccountKeys::encryption,
                                              QString::number(EncryptionMethod::SSL)).toInt();
}

AccountConfiguration::ServiceType AccountConfiguration::serviceType() const
{
    if (m_serviceType == QStringLiteral("smtp")) {
        return SMTP;
    } else if (m_serviceType == QStringLiteral("pop3")) {
        return POP3;
    } else {
        return IMAP;
    }
}

bool AccountConfiguration::acceptUntrustedCertificates() const
{
    return (m_service->value(AccountKeys::acceptUntrustedCert, "0").toInt() != 0);
}

QString AccountConfiguration::customValue(const QString &ns, const QString &key)
{
    return m_service->value(QString("%1.%2").arg(ns, key));
}

void AccountConfiguration::setCustomValue(const QString &ns, const QString &key, const QString &value)
{
    const QString nskey = QString("%1.%2").arg(ns, key);
    m_service->setValue(nskey, value);
    emit customValueChanged(nskey, value);
}

void AccountConfiguration::setName(const QString &name)
{
    m_service->setValue(AccountKeys::name, name);
    emit nameChanged(name);
}

void AccountConfiguration::setEmail(const QString &email)
{
    m_service->setValue(AccountKeys::email, email);
    emit emailChanged(email);
}

void AccountConfiguration::setPassword(const QString &pwd)
{
    if (serviceType() == SMTP) {
        m_service->setValue(AccountKeys::smtppassword, PasswordHelper::encodePassword(pwd));
    } else {
        m_service->setValue(AccountKeys::password, PasswordHelper::encodePassword(pwd));
    }
    emit passwordChanged();
}

void AccountConfiguration::setServer(const QString &server)
{
    m_service->setValue(AccountKeys::server, server);
    emit serverChanged(server);
}

void AccountConfiguration::setPort(const QString &port)
{
    m_service->setValue(AccountKeys::port, port);
    emit portChanged(port);
}

void AccountConfiguration::setEncryption(AccountConfiguration::EncryptionMethod method)
{
    m_service->setValue(AccountKeys::encryption, QString::number(method));
    emit encryptionChanged();
}

void AccountConfiguration::setAcceptUntrustedCertificates(const bool accept)
{
    m_service->setValue(AccountKeys::acceptUntrustedCert, QString::number(accept ? 1 : 0));
    emit acceptUntrustedCertChanged();
}

void AccountConfiguration::init(QMailAccountConfiguration *config, const QString &serviceType) {
    m_config = config;
    m_serviceType = serviceType;
    m_service = new QMailServiceConfiguration(m_config, m_serviceType);
    m_service->setType((m_serviceType == QStringLiteral("smtp")) ?
                           QMailServiceConfiguration::Sink :
                           QMailServiceConfiguration::Source);
    m_service->setVersion(100);
}

void AccountConfiguration::emitConfigurationChanges(){
    emit nameChanged(name());
    emit emailChanged(email());
    emit serverChanged(server());
    emit portChanged(port());
    emit encryptionChanged();
    emit passwordChanged();
    emit acceptUntrustedCertChanged();
}

ImapAccountConfiguration::ImapAccountConfiguration(QObject *parent) : AccountConfiguration(parent){}

AccountConfiguration::SaslMechanism ImapAccountConfiguration::saslMechanism() const
{
    return (AccountConfiguration::SaslMechanism)m_service->value(AccountKeys::saslMechanism, "0").toInt();
}

bool ImapAccountConfiguration::canDeleteMail() const
{
    return (m_service->value(AccountKeys::canDeleteMail, "1").toInt() != 0);
}

bool ImapAccountConfiguration::downloadAttachments() const
{
    return m_service->value(AccountKeys::downloadAttachments, "0").toInt() != 0;
}

bool ImapAccountConfiguration::isAutoDownload() const
{
    return m_service->value(AccountKeys::autoDownload, "0").toInt() != 0;
}

int ImapAccountConfiguration::maxMailSize() const
{
    return m_service->value(AccountKeys::maxMailSize, QStringLiteral("100")).toInt();
}

ImapAccountConfiguration::SubType ImapAccountConfiguration::preferredSubType() const
{
    if (m_service->value(AccountKeys::prefferedSubType, QStringLiteral("html")) == QStringLiteral("html"))
        return SubType::Html;
    else
        return SubType::Plain;
}

bool ImapAccountConfiguration::canIDLE() const
{
    return m_service->value(AccountKeys::canIDLE, QStringLiteral("0")).toInt() != 0;
}

bool ImapAccountConfiguration::idleEnabled() const
{
    return m_service->value(AccountKeys::idleEnabled, QStringLiteral("1")).toInt() != 0;
}

QString ImapAccountConfiguration::baseFolder() const
{
    return m_service->value(AccountKeys::baseFolder, QStringLiteral(""));
}

QStringList ImapAccountConfiguration::pushFolders() const
{
    return m_service->value(AccountKeys::pushFolders).split(QChar('\x0A'), QString::SkipEmptyParts);
}

int ImapAccountConfiguration::checkInterval() const
{
    return m_service->value(AccountKeys::checkInterval, QStringLiteral("5")).toInt();
}

bool ImapAccountConfiguration::checkWhenRoaming() const
{
    return m_service->value(AccountKeys::checkWhenRoaming, QStringLiteral("0")).toInt() != 0;
}

QStringList ImapAccountConfiguration::capabilities() const
{
    return m_service->value(AccountKeys::capabilities).split(QChar(' '), QString::SkipEmptyParts);
}

int ImapAccountConfiguration::timeTillLogout() const
{
    const int tenSeconds(10 * 1000);
    QString t(m_service->value(AccountKeys::timeTillLogout, QString::number(tenSeconds)));

    bool ok;
    int val(t.toInt(&ok));
    if (!ok) {
        qCWarning(D_ACCOUNT_CONF) << "Could not parse timeTillLogout";
        return tenSeconds;
    } else {
        return val;
    }
}

int ImapAccountConfiguration::searchLimit() const
{
    const int defaultLimit = 100;
    QString t(m_service->value(AccountKeys::searchLimit, QString::number(defaultLimit)));
    bool ok;
    int val(t.toInt(&ok));
    if (!ok) {
        qCWarning(D_ACCOUNT_CONF) << "Could not parse searchLimit";
        return 0;
    } else {
        return val;
    }
}

// setters

void ImapAccountConfiguration::setSaslMechanism(AccountConfiguration::SaslMechanism sasl)
{
    m_service->setValue(AccountKeys::saslMechanism, QString::number(sasl));
    emit saslChanged();
}

void ImapAccountConfiguration::setDeleteMail(const bool canDelete)
{
    m_service->setValue(AccountKeys::canDeleteMail, QString::number(canDelete ? 1 : 0));
}

void ImapAccountConfiguration::setDownloadAttachments(const bool download)
{
    m_service->setValue(AccountKeys::downloadAttachments, QString::number(download ? 1 : 0));
    emit downloadAttachmentsChanged();
}

void ImapAccountConfiguration::setAutoDownload(const bool autoDownload)
{
    m_service->setValue(AccountKeys::autoDownload, QString::number(autoDownload ? 1 : 0));
    emit autoDownloadChanged();
}

void ImapAccountConfiguration::setMaxMailSize(const int &size)
{
    m_service->setValue(AccountKeys::maxMailSize, QString::number(size));
    emit maxMailSizeChanged();
}

void ImapAccountConfiguration::setPreferredSubType(ImapAccountConfiguration::SubType subType)
{
    m_service->setValue(AccountKeys::prefferedSubType, QString((subType == SubType::Html) ? "html" : "plain"));
    emit preferredSubTypeChanged();
}

void ImapAccountConfiguration::setIdleEnabled(const bool enabled)
{
    m_service->setValue(AccountKeys::idleEnabled, QString::number(enabled ? 1 : 0));
    emit idleEnabledChanged();
}

void ImapAccountConfiguration::setBaseFolder(const QString &baseFolder)
{
    m_service->setValue(AccountKeys::baseFolder, baseFolder);
    emit baseFolderChanged();
}

void ImapAccountConfiguration::setPushFolders(const QStringList &pushFolders)
{
    m_service->setValue(AccountKeys::pushFolders, QString("") + pushFolders.join(QChar('\x0A')));
    emit pushFolderChanged();
}

void ImapAccountConfiguration::setCheckInterval(const int &interval)
{
    m_service->setValue(AccountKeys::checkInterval, QString::number(interval));
    emit checkIntervalChanged();
}

void ImapAccountConfiguration::setCheckWhenRoaming(const bool check)
{
    m_service->setValue(AccountKeys::checkWhenRoaming, QString::number(check ? 1 : 0));
    emit checkWhenRoamingChanged();
}

void ImapAccountConfiguration::setSearchLimit(const int &limit)
{
    m_service->setValue(AccountKeys::searchLimit, QString::number(limit));
    emit searchLimitChanged();
}

void ImapAccountConfiguration::emitConfigurationChanges() {
    emit saslChanged();
    emit canDeleteMailChanged();
    emit downloadAttachmentsChanged();
    emit autoDownloadChanged();
    emit maxMailSizeChanged();
    emit preferredSubTypeChanged();
    emit idleEnabledChanged();
    emit baseFolderChanged();
    emit pushFolderChanged();
    emit checkIntervalChanged();
    emit checkWhenRoamingChanged();
    emit searchLimitChanged();
    AccountConfiguration::emitConfigurationChanges();
}

SmtpAccountConfiguration::SmtpAccountConfiguration(QObject *parent) : AccountConfiguration(parent){}

AccountConfiguration::SaslMechanism SmtpAccountConfiguration::saslMechanism() const
{
    return (AccountConfiguration::SaslMechanism)m_service->value(AccountKeys::saslMechanism, "0").toInt();
}

QString SmtpAccountConfiguration::username() const
{
    return m_service->value(AccountKeys::username);
}

bool SmtpAccountConfiguration::authFromCaps() const
{
    return m_service->value(AccountKeys::authFromCaps, QStringLiteral("0")).toInt() != 0;
}

QString SmtpAccountConfiguration::initials()
{
    QString name = this->name();
    if (name.isEmpty() || !name.at(0).isLetter()) {
        // Name is empty so return first character of address. :-/
        return QString(email().at(0).toUpper());
    }
    // Intitials string
    QString initials;
    // Now break up the name, we have to set the encoding here as QT_NO_CAST_FROM/TO_ASCII is set
    QStringList parts = name.split(QStringLiteral(" "));
    if (parts.first().at(0).isLetter()) {
        initials += parts.first().at(0).toUpper();
    }
    if (parts.size() > 1) {
        if (parts.last().at(0).isLetter()) {
            initials += parts.last().at(0).toUpper();
        }
    }
    return initials;
}

QString SmtpAccountConfiguration::signature() const
{
    static const QString defaultSig = QStringLiteral("-- \n%1").arg(tr("Sent using Dekko from my Ubuntu device"));
    return m_service->value(AccountKeys::signature, defaultSig);
}

void SmtpAccountConfiguration::setSaslMechanism(AccountConfiguration::SaslMechanism sasl)
{
    m_service->setValue(AccountKeys::saslMechanism, QString::number(sasl));
    emit saslChanged();
}

void SmtpAccountConfiguration::setUsername(const QString &username)
{
    m_service->setValue(AccountKeys::username, username);
    emit usernameChanged();
}

void SmtpAccountConfiguration::setAuthFromCaps(const bool use)
{
    m_service->setValue(AccountKeys::authFromCaps, QString::number(use ? 1 : 0));
    emit authFromCapsChanged();
}

void SmtpAccountConfiguration::setSignature(const QString &sig)
{
    m_service->setValue(AccountKeys::signature, sig);
    emit signatureChanged();
}

void SmtpAccountConfiguration::emitConfigurationChanges() {
    emit saslChanged();
    emit usernameChanged();
    emit authFromCapsChanged();
    emit signatureChanged();
    AccountConfiguration::emitConfigurationChanges();
}

PopAccountConfiguration::PopAccountConfiguration(QObject *parent) : AccountConfiguration(parent){}

bool PopAccountConfiguration::canDeleteMail() const
{
    return m_service->value(AccountKeys::canDeleteMail, QStringLiteral("1")).toInt() != 0;
}

bool PopAccountConfiguration::isAutoDownload() const
{
    return m_service->value(AccountKeys::autoDownload, QStringLiteral("0")).toInt() != 0;
}

int PopAccountConfiguration::maxMailSize() const
{
    return m_service->value(AccountKeys::maxMailSize, QStringLiteral("100")).toInt();
}

int PopAccountConfiguration::checkInterval() const
{
    return m_service->value(AccountKeys::checkInterval, QStringLiteral("-1")).toInt();
}

bool PopAccountConfiguration::checkWhenRoaming() const
{
    return m_service->value(AccountKeys::checkWhenRoaming, QStringLiteral("0")).toInt() != 0;
}

void PopAccountConfiguration::setDeleteMail(const bool setDelete)
{
    m_service->setValue(AccountKeys::canDeleteMail, QString::number(setDelete ? 1 : 0));
    emit canDeleteMailChanged();
}

void PopAccountConfiguration::setAutoDownload(const bool autoDownload)
{
    m_service->setValue(AccountKeys::autoDownload, QString::number(autoDownload ? 1 : 0));
    emit autoDownloadChanged();
}

void PopAccountConfiguration::setMaxMailSize(const int &maxSize)
{
    m_service->setValue(AccountKeys::maxMailSize, QString::number(maxSize));
    emit maxMailSizeChanged();
}

void PopAccountConfiguration::setCheckInterval(const int &interval)
{
    m_service->setValue(AccountKeys::checkInterval, QString::number(interval));
    emit checkIntervalChanged();
}

void PopAccountConfiguration::setCheckWhenRoaming(const bool check)
{
    m_service->setValue(AccountKeys::checkWhenRoaming, QString::number(check ? 1 : 0));
    emit checkWhenRoamingChanged();
}

void PopAccountConfiguration::emitConfigurationChanges() {
    emit canDeleteMailChanged();
    emit autoDownloadChanged();
    emit maxMailSizeChanged();
    emit checkIntervalChanged();
    emit checkWhenRoamingChanged();
    AccountConfiguration::emitConfigurationChanges();
}
