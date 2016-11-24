/**************************************************************************
   Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
**************************************************************************/
#include "UOABearerPlugin.h"
#include <QByteArray>

UOABearerPlugin::UOABearerPlugin(QObject *parent) : UOAService(parent)
{
    setSessionType("oauth2");
}

QString UOABearerPlugin::name() const
{
    return QStringLiteral("oauthbearer");
}

UOAService *UOABearerPlugin::create(const quint32 &uoaId)
{
    UOABearerPlugin *s = new UOABearerPlugin(this);
    s->setUOAAccountId(uoaId);
    return s;
}

QByteArray UOABearerPlugin::process(const SignOn::SessionData &data, const QString &ua)
{
    m_data = data.data<OAuth2PluginNS::OAuth2PluginData>();
    return UOAService::process(data, ua);
}

QByteArray UOABearerPlugin::processServerResponse(const QByteArray &response)
{
    Q_UNUSED(response);
    return buildTokenResponse();
}

void UOABearerPlugin::onResponseReceived(const SignOn::SessionData &data)
{
    m_response.clear();
    m_data = data.data<OAuth2PluginNS::OAuth2PluginData>();
    m_tokenData = data.data<OAuth2PluginNS::OAuth2PluginTokenData>();

    if (!m_tokenData.AccessToken().isEmpty()) {
        if (!m_initialResponseSent) {
            if (m_protocol == QStringLiteral("imap")) {
                QByteArray mech(getMechFromCaps());
                if (!mech.isEmpty()) {
                    m_response = QByteArrayLiteral("AUTHENTICATE ") + mech;
                    if (m_caps.contains(QStringLiteral("SASL-IR"))) {
                        m_response += " " + buildTokenResponse();
                    }
                }
            } else if (m_protocol == QStringLiteral("smtp")){
                m_response = QByteArrayLiteral("AUTH ") + getMechFromCaps();
                m_response += " " + buildTokenResponse();
            }
            m_initialResponseSent = true;
        }
    }
    m_responseReady = true;
    emit response(m_response);
}

QByteArray UOABearerPlugin::getMechFromCaps()
{
    if (m_caps.contains(QStringLiteral("OAUTHBEARER"))) {
        return QByteArrayLiteral("OAUTHBEARER");
    } else if (m_caps.contains(QStringLiteral("XOAUTH2"))) {
        return QByteArrayLiteral("XOAUTH2");
    }
    return QByteArray();
}

QByteArray UOABearerPlugin::buildTokenResponse()
{
    QString username;
    if (m_protocol == QStringLiteral("imap")) {
        username = m_config.value(QStringLiteral("username"));
    } else {
        username = m_config.value(QStringLiteral("smtpusername"));
    }
    QByteArray response;
    response.append("user=");
    response.append(username.toUtf8());
    response.append("\x01");
    response.append("auth=Bearer ");
    response.append(m_tokenData.AccessToken().toUtf8());
    response.append("\x01\x01");
    return response.toBase64();
}
