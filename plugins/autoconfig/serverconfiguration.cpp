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
#include "serverconfiguration.h"
#include <QMetaEnum>
#include <QMetaObject>

ServerConfiguration::ServerConfiguration(QObject *parent) :
    QObject(parent),
    m_imapPort(0),
    m_smtpPort(0),
    m_imapMethod(NetworkMethod::INVALID),
    m_smtpMethod(NetworkMethod::INVALID)
{
}

QString ServerConfiguration::imapHost() const
{
    return m_imapHost;
}

void ServerConfiguration::setImapHost(const QString &hostname)
{
    if (hostname.isEmpty()) {
        return;
    }
    if (hostname != m_imapHost) {
        m_imapHost = hostname;
        emit configChanged();
    }
}

int ServerConfiguration::imapPort() const
{
    return m_imapPort;
}

void ServerConfiguration::setImapPort(const int &port)
{
    if (port != m_imapPort) {
        m_imapPort = port;
        emit configChanged();
    }
}

ServerConfiguration::NetworkMethod ServerConfiguration::imapMethod() const
{
    return m_imapMethod;
}

void ServerConfiguration::setImapMethod(const ServerConfiguration::NetworkMethod &method)
{
    if (method != m_imapMethod) {
        m_imapMethod = method;
        emit configChanged();
    }
}

QString ServerConfiguration::smtpHost() const
{
    return m_smtpHost;
}

void ServerConfiguration::setSmtpHost(const QString &hostname)
{
    if (hostname.isEmpty()) {
        return;
    }
    if (hostname != m_smtpHost) {
        m_smtpHost = hostname;
        emit configChanged();
    }
}

int ServerConfiguration::smtpPort() const
{
    return m_smtpPort;
}

void ServerConfiguration::setSmtpPort(const int &port)
{
    if (port != m_smtpPort) {
        m_smtpPort = port;
        emit configChanged();
    }
}

ServerConfiguration::NetworkMethod ServerConfiguration::smtpMethod() const
{
    return m_smtpMethod;
}

void ServerConfiguration::setSmtpMethod(const ServerConfiguration::NetworkMethod &method)
{
    if (method != m_smtpMethod) {
        m_smtpMethod = method;
        emit configChanged();
    }
}

bool ServerConfiguration::isValid() const
{
    bool valid = true;
    if (m_imapHost.isEmpty() || m_smtpHost.isEmpty()) {
        valid = false;
    }
    if (m_imapPort == 0 || m_smtpPort == 0) {
        valid = false;
    }
    if (m_imapMethod == NetworkMethod::INVALID || m_smtpMethod == NetworkMethod::INVALID) {
        valid = false;
    }
    return valid;

}

QVariantMap ServerConfiguration::toMap() const
{
    if (!isValid()) {
        return QVariantMap();
    }

    auto networkMethodToString = [](const NetworkMethod method) -> QString {
        const QMetaObject &metaObj = ServerConfiguration::staticMetaObject;
        int index = metaObj.indexOfEnumerator("NetworkMethod");
        QMetaEnum metaEnum = metaObj.enumerator(index);
        return metaEnum.valueToKey(method);
    };

    QVariantMap imapconfig;
    imapconfig["hostname"] = m_imapHost;
    imapconfig["port"] = m_imapPort;
    imapconfig["method"] = networkMethodToString(m_imapMethod);
    QVariantMap smtpconfig;
    smtpconfig["hostname"] = m_smtpHost;
    smtpconfig["port"] = m_smtpPort;
    smtpconfig["method"] = networkMethodToString(m_smtpMethod);
    QVariantMap map;
    map["imap"] = imapconfig;
    map["smtp"] = smtpconfig;
    return map;
}

bool ServerConfiguration::operator ==(const ServerConfiguration *config) const
{
    return (config->imapHost() == m_imapHost
            && config->imapPort() == m_imapPort
            && config->imapMethod() == m_imapMethod
            && config->smtpHost() == m_smtpHost
            && config->smtpPort() == m_smtpPort
            && config->smtpMethod() == m_smtpMethod);
}
