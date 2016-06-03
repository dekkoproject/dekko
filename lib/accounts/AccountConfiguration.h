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
#ifndef ACCOUNTCONFIGURATION_H
#define ACCOUNTCONFIGURATION_H

#include <QObject>
#include <qmailaccountconfiguration.h>
#include <qmailserviceconfiguration.h>
#include <qmailtransport.h>
#include <qmailnamespace.h>

struct AccountKeys {
    // Base properties
    static const QString name, email, password, server, port, encryption, acceptUntrustedCert;
    //IMAP/POP3 properties
    static const QString canDeleteMail, downloadAttachments, autoDownload, maxMailSize;
    static const QString prefferedSubType, canIDLE, idleEnabled, baseFolder, pushFolders, checkInterval;
    static const QString checkWhenRoaming, capabilities, timeTillLogout, searchLimit;
    //SMTP
    static const QString saslMechanism, username, authFromCaps, smtppassword;
};

class AccountConfiguration : public QObject
{
    Q_OBJECT
    /** short Base properties supported by all service configurations */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString server READ server WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY(QString port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(EncryptionMethod encryption READ encryption WRITE setEncryption NOTIFY encryptionChanged)
    Q_PROPERTY(ServiceType serviceType READ serviceType CONSTANT)
    Q_PROPERTY(bool acceptUntrustedCertificates READ acceptUntrustedCertificates WRITE setAcceptUntrustedCertificates NOTIFY acceptUntrustedCertChanged)

    Q_ENUMS(ServiceType)
    Q_ENUMS(EncryptionMethod)
    Q_ENUMS(SaslMechanism)

public:
    explicit AccountConfiguration(QObject *parent = 0);
    AccountConfiguration(QObject *parent, QMailAccountConfiguration *config, const QString &serviceType);

    enum ServiceType {
        IMAP,
        POP3,
        SMTP
    };

    enum EncryptionMethod {
        None = QMailTransport::Encrypt_NONE,
        SSL = QMailTransport::Encrypt_SSL,
        STARTTLS = QMailTransport::Encrypt_TLS
    };

    enum SaslMechanism {
        NONE = QMail::NoMechanism,
        LOGIN = QMail::LoginMechanism,
        PLAIN = QMail::PlainMechanism,
        CRAM_MD5 = QMail::CramMd5Mechanism
    };

    QString name() const;
    QString email() const;
    QString password() const;
    QString server() const;
    QString port() const;
    EncryptionMethod encryption() const;
    ServiceType serviceType() const;
    bool acceptUntrustedCertificates() const;

signals:
    void nameChanged(QString name);
    void emailChanged(QString email);
    void serverChanged(QString server);
    void portChanged(QString port);
    void encryptionChanged();
    void passwordChanged();
    void acceptUntrustedCertChanged();

public slots:
    void setName(const QString &name);
    void setEmail(const QString &email);
    void setPassword(const QString &pwd);
    void setServer(const QString &server);
    void setPort(const QString &port);
    void setEncryption(EncryptionMethod method);
    void setAcceptUntrustedCertificates(const bool accept);

protected:
    void init(QMailAccountConfiguration *config, const QString &serviceType);
    void addServiceType(ServiceType type);
    void removeServiceType(ServiceType type);
    QMailAccountConfiguration *m_config;
    QMailServiceConfiguration *m_service;
    QString m_serviceType;
};

class ImapAccountConfiguration : public AccountConfiguration
{
    Q_OBJECT
    Q_PROPERTY(SaslMechanism saslMechanism READ saslMechanism WRITE setSaslMechanism NOTIFY saslChanged)
    Q_PROPERTY(bool canDeleteMail READ canDeleteMail WRITE setDeleteMail NOTIFY canDeleteMailChanged)
    Q_PROPERTY(bool downloadAttachments READ downloadAttachments WRITE setDownloadAttachments NOTIFY downloadAttachmentsChanged)
    Q_PROPERTY(bool isAutoDownload READ isAutoDownload WRITE setAutoDownload NOTIFY autoDownloadChanged)
    Q_PROPERTY(int maxMailSize READ maxMailSize WRITE setMaxMailSize NOTIFY maxMailSizeChanged)
    Q_PROPERTY(SubType preferredSubType READ preferredSubType WRITE setPreferredSubType NOTIFY preferredSubTypeChanged)
    Q_PROPERTY(bool canIDLE READ canIDLE CONSTANT)
    Q_PROPERTY(bool idleEnabled READ idleEnabled WRITE setIdleEnabled NOTIFY idleEnabledChanged)
    Q_PROPERTY(QString baseFolder READ baseFolder WRITE setBaseFolder NOTIFY baseFolderChanged)
    Q_PROPERTY(QStringList pushFolders READ pushFolders WRITE setPushFolders NOTIFY pushFolderChanged)
    Q_PROPERTY(int checkInterval READ checkInterval WRITE setCheckInterval NOTIFY checkIntervalChanged)
    Q_PROPERTY(bool checkWhenRoaming READ checkWhenRoaming WRITE setCheckWhenRoaming NOTIFY checkWhenRoamingChanged)
    Q_PROPERTY(QStringList capabilities READ capabilities CONSTANT)
    Q_PROPERTY(int timeTillLogout READ timeTillLogout CONSTANT)
    Q_PROPERTY(int searchLimit READ searchLimit WRITE setSearchLimit NOTIFY searchLimitChanged)
    Q_ENUMS(SubType)

public:
    explicit ImapAccountConfiguration(QObject *parent = 0) : AccountConfiguration(parent){}
    ImapAccountConfiguration(QObject *parent, QMailAccountConfiguration *config, const QString &serviceType) :
        AccountConfiguration(parent, config, serviceType){}

    enum SubType {
        Plain,
        Html
    };

    SaslMechanism saslMechanism() const;
    bool canDeleteMail() const;
    bool downloadAttachments() const;
    bool isAutoDownload() const;
    int maxMailSize() const;
    SubType preferredSubType() const;
    bool canIDLE() const;
    bool idleEnabled() const;
    QString baseFolder() const;
    QStringList pushFolders() const;
    int checkInterval() const;
    bool checkWhenRoaming() const;
    QStringList capabilities() const;
    int timeTillLogout() const;
    int searchLimit() const;

signals:
    void saslChanged();
    void canDeleteMailChanged();
    void downloadAttachmentsChanged();
    void autoDownloadChanged();
    void maxMailSizeChanged();
    void preferredSubTypeChanged();
    void idleEnabledChanged();
    void baseFolderChanged();
    void pushFolderChanged();
    void checkIntervalChanged();
    void checkWhenRoamingChanged();
    void searchLimitChanged();

public slots:
    void setSaslMechanism(SaslMechanism sasl);
    void setDeleteMail(const bool canDelete);
    void setDownloadAttachments(const bool download);
    void setAutoDownload(const bool autoDownload);
    void setMaxMailSize(const int &size);
    void setPreferredSubType(SubType subType);
    void setIdleEnabled(const bool enabled);
    void setBaseFolder(const QString &baseFolder);
    void setPushFolders(const QStringList &pushFolders);
    void setCheckInterval(const int &interval); // in milliseconds
    void setCheckWhenRoaming(const bool check);
    void setSearchLimit(const int &limit);
};

class SmtpAccountConfiguration : public AccountConfiguration
{
    Q_OBJECT
    Q_PROPERTY(SaslMechanism saslMechanism READ saslMechanism WRITE setSaslMechanism NOTIFY saslChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString initials READ initials CONSTANT)
    Q_PROPERTY(bool authFromCaps READ authFromCaps WRITE setAuthFromCaps NOTIFY authFromCapsChanged)

public:
    explicit SmtpAccountConfiguration(QObject *parent = 0) : AccountConfiguration(parent){}
    SmtpAccountConfiguration(QObject *parent, QMailAccountConfiguration *config, const QString &serviceType) :
        AccountConfiguration(parent, config, serviceType){}

    SaslMechanism saslMechanism() const;
    QString username() const;
    bool authFromCaps() const;
    QString initials();

signals:
    void saslChanged();
    void usernameChanged();
    void authFromCapsChanged();

public slots:
    void setSaslMechanism(SaslMechanism sasl);
    void setUsername(const QString &username);
    void setAuthFromCaps(const bool use);

};

class PopAccountConfiguration : public AccountConfiguration
{
    Q_OBJECT
    Q_PROPERTY(bool canDeleteMail READ canDeleteMail WRITE setDeleteMail NOTIFY canDeleteMailChanged)
    Q_PROPERTY(bool isAutoDownload READ isAutoDownload WRITE setAutoDownload NOTIFY autoDownloadChanged)
    Q_PROPERTY(int maxMailSize READ maxMailSize WRITE setMaxMailSize NOTIFY maxMailSizeChanged)
    Q_PROPERTY(int checkInterval READ checkInterval WRITE setCheckInterval NOTIFY checkIntervalChanged)
    Q_PROPERTY(bool checkWhenRoaming READ checkWhenRoaming WRITE setCheckWhenRoaming NOTIFY checkWhenRoamingChanged)

public:
    explicit PopAccountConfiguration(QObject *parent = 0) : AccountConfiguration(parent){}
    PopAccountConfiguration(QObject *parent, QMailAccountConfiguration *config, const QString &serviceType) :
        AccountConfiguration(parent, config, serviceType){}

    bool canDeleteMail() const;
    bool isAutoDownload() const;
    int maxMailSize() const;
    int checkInterval() const;
    bool checkWhenRoaming() const;

signals:
    void canDeleteMailChanged();
    void autoDownloadChanged();
    void maxMailSizeChanged();
    void checkIntervalChanged();
    void checkWhenRoamingChanged();

public slots:
    void setDeleteMail(const bool setDelete);
    void setAutoDownload(const bool autoDownload);
    void setMaxMailSize(const int &maxSize);
    void setCheckInterval(const int &interval);
    void setCheckWhenRoaming(const bool check);

};

class PasswordHelper : public QMailServiceConfiguration
{
public:
    static QString decodePassword(const QString &value) {
        return decodeValue(value);
    }
    static QString encodePassword(const QString &value) {
        return encodeValue(value);
    }
};

#endif // ACCOUNTCONFIGURATION_H
