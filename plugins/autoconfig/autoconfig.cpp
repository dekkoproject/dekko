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

AutoConfig::AutoConfig(QObject *parent, EmailProvider *config):
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
        EmailProvider *provider = EmailProvider::fromXml(reply->readAll());
        successful = provider->isValid();
        if (successful) {
            m_config = provider;
            emit success(m_config);
        } else {
            emit failed();
        }
    }

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

    m_config = EmailProvider::fromXml(fakeXmlResponse);
    emit success(m_config);
}


