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
#include "autoconfig.h"
#include <QDebug>
#include <QDomDocument>

AutoConfig::AutoConfig(QObject *parent, ServerConfiguration *config):
    QObject(parent), m_config(config), m_netStatus(new NetworkingStatus)
{
    m_nam = new QNetworkAccessManager(this);
    connect(m_nam, &QNetworkAccessManager::finished, this, &AutoConfig::handleRequestResponse);
}

void AutoConfig::lookUp(const QUrl &url)
{
    if (!url.isValid()) {
        qDebug() << "[AutoConfig] Invalid URL: " << url;
        emit failed();
        return;
    }
    m_nam->get(QNetworkRequest(url));
}

bool AutoConfig::networkAccessible() const
{
    if (qgetenv("QT_QPA_PLATFORM") != "ubuntumirclient" ) {
        return m_nam->networkAccessible() == QNetworkAccessManager::Accessible;
    } else {
        return m_netStatus.data()->status() == NetworkingStatus::Offline;
    }
}

void AutoConfig::handleRequestResponse(QNetworkReply *reply)
{
    // We should handle redirects so do that first.
    QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (redirect.isValid()) {
        qDebug() << "[AutoConfig] Redirecting to: " << redirect.toUrl();
        m_nam->get(QNetworkRequest(redirect.toUrl()));
        reply->deleteLater();
        return;
    }
    bool successful = false;
    if (reply->error() == QNetworkReply::NoError) {
        successful = parseResponse(reply->readAll());
    }
    if (successful) {
        emit success(m_config);
    } else {
        emit failed();
    }
}

bool AutoConfig::parseResponse(const QByteArray &response)
{
    bool successful = false;
    successful = configFromResponse(response, "incomingServer", "imap");
    // Only look for smtp if we found imap info.
    if (successful) {
        successful = configFromResponse(response, "outgoingServer", "smtp");
    }
    return successful;
}

bool AutoConfig::configFromResponse(const QByteArray &response, const QString &tagName, const QString &protocol)
{
    QDomDocument doc;
    doc.setContent(QString(response));
    QDomNodeList nodeList = doc.elementsByTagName(tagName);
    if (nodeList.isEmpty()) {
        qDebug() << "[AutoConfig] No server settings for " << tagName;
        return false;
    }
    QDomElement *server = 0;
    for (int i = 0; i < nodeList.size(); ++i) {
        if (nodeList.at(i).attributes().namedItem("type").nodeValue() == protocol) {
            server = new QDomElement(nodeList.at(i).toElement());
            break;
        }
    }
    if (server == 0) {
        qDebug() << "[AutoConfig] No server settings for " << protocol;
        delete server;
        return false;
    } else {

        auto networkMethodFromString = [](const QString &socketType) {
            if (socketType == "SSL") {
                return ServerConfiguration::NetworkMethod::SSL_TLS;
            } else if (socketType == "STARTTLS") {
                return ServerConfiguration::NetworkMethod::STARTTLS;
            } else {
                return ServerConfiguration::NetworkMethod::CLEAR_TEXT;
            }
        };

        if (protocol == "imap") {
            m_config->setImapHost(server->elementsByTagName("hostname").at(0).firstChild().nodeValue());
            m_config->setImapPort(server->elementsByTagName("port").at(0).firstChild().nodeValue().toInt());
            m_config->setImapMethod(networkMethodFromString(server->elementsByTagName("socketType").at(0).firstChild().nodeValue()));
        } else if (protocol == "smtp") {
            m_config->setSmtpHost(server->elementsByTagName("hostname").at(0).firstChild().nodeValue());
            m_config->setSmtpPort(server->elementsByTagName("port").at(0).firstChild().nodeValue().toInt());
            m_config->setSmtpMethod(networkMethodFromString(server->elementsByTagName("socketType").at(0).firstChild().nodeValue()));
        } else {
            // NOTE: We will most likely want to add pop settings to this at a later date.
            qDebug() << "[AutoConfig] Unknown protocol: " << protocol;
            delete server;
            return false;
        }
    }
    delete server;
    return true;
}

void AutoConfig::fakeLookUp(const QUrl &url)
{
    if (url.isEmpty()) {
        return;
    }
    QByteArray fakeXmlResponse = "\
        <clientConfig version=\"1.1\"> \
            <emailProvider id=\"example.com\"> \
            <incomingServer type=\"imap\"> \
                <hostname>imap.example.com</hostname> \
                <port>993</port> \
                <socketType>SSL</socketType> \
                <authentication>password-cleartext</authentication> \
                <username>%EMAILADDRESS%</username> \
            </incomingServer> \
            <outgoingServer type=\"smtp\"> \
                <hostname>smtp.example.com</hostname> \
                <port>587</port> \
                <socketType>STARTTLS</socketType> \
                <authentication>password-cleartext</authentication> \
                <username>%EMAILADDRESS%</username> \
            </outgoingServer> \
        </clientConfig>";

    if (parseResponse(fakeXmlResponse)) {
        emit success(m_config);
    }
}

