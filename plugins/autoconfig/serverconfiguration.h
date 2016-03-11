/* Copyright (C) 2015 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef SERVERCONFIGURATION_H
#define SERVERCONFIGURATION_H

#include <QObject>
#include <QVariantMap>

class ServerConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString imapHostName READ imapHost NOTIFY configChanged )
    Q_PROPERTY(int imapPort READ imapPort NOTIFY configChanged )
    Q_PROPERTY(NetworkMethod imapMethod READ imapMethod NOTIFY configChanged)
    Q_PROPERTY(QString smtpHostName READ smtpHost NOTIFY configChanged )
    Q_PROPERTY(int smtpPort READ smtpPort NOTIFY configChanged )
    Q_PROPERTY(NetworkMethod smtpMethod READ smtpMethod NOTIFY configChanged )
    Q_PROPERTY(bool isValid READ isValid NOTIFY configChanged)
    Q_ENUMS(NetworkMethod)
    Q_ENUMS(DefaultPorts)

public:
    explicit ServerConfiguration(QObject *parent = 0);

    enum NetworkMethod {
        INVALID, /**< No configuration was provided */
        CLEAR_TEXT, /**< Cleartext connection over network -- no encryption whatsoever */
        STARTTLS, /**< Network connection which starts in plaintext and is upgraded via STARTTLS later on */
        SSL_TLS, /**< Network connection over SSL/TLS encrypted from the very beginning */
    };

    enum DefaultPorts {
        PORT_IMAP = 143,
        PORT_IMAPS = 993,
        PORT_SMTP = 587,
        PORT_SSMTP = 465
    };

    // IMAP configs
    QString imapHost() const;
    void setImapHost(const QString &hostname);
    int imapPort() const;
    void setImapPort(const int &port);
    NetworkMethod imapMethod() const;
    void setImapMethod(const NetworkMethod &method);

    //SMTP configs
    QString smtpHost() const;
    void setSmtpHost(const QString &hostname);
    int smtpPort() const;
    void setSmtpPort(const int &port);
    NetworkMethod smtpMethod() const;
    void setSmtpMethod(const NetworkMethod &method);

    bool isValid() const;
    Q_INVOKABLE QVariantMap toMap() const;

    bool operator ==(const ServerConfiguration *config) const;

signals:
    void configChanged();

private:
    QString m_imapHost;
    QString m_smtpHost;
    int m_imapPort;
    int m_smtpPort;
    NetworkMethod m_imapMethod;
    NetworkMethod m_smtpMethod;

};

#endif // SERVERCONFIGURATION_H
