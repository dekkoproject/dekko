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
#include "UOASaslPlugin.h"

UOASaslPlugin::UOASaslPlugin(QObject *parent) : UOAService(parent),
    m_initialResponseSent(false)
{
    setSessionType(QStringLiteral("sasl"));
}

QString UOASaslPlugin::name() const
{
    return QStringLiteral("sasl");
}

UOAService *UOASaslPlugin::create(const quint32 &uoaId)
{
    UOASaslPlugin *s = new UOASaslPlugin(this);
    s->setUOAAccountId(uoaId);
    return s;
}

QByteArray UOASaslPlugin::process(const SignOn::SessionData &data, const QString &mech)
{
    m_saslData = data.data<SaslPluginNS::SaslData>();
    return UOAService::process(m_saslData, mech);
}

QByteArray UOASaslPlugin::processServerResponse(const QByteArray &response)
{
    m_saslData.setChallenge(response);
    return process(m_saslData, m_saslData.ChosenMechanism());
}

void UOASaslPlugin::onResponseReceived(const SignOn::SessionData &data)
{
    m_response.clear();
    m_sessionData = data;
    m_saslData = m_sessionData.data<SaslPluginNS::SaslData>();
    if (!m_saslData.Response().isEmpty()) {
        if (!m_initialResponseSent) {
            if (m_protocol == QStringLiteral("imap")) {
                m_response = QByteArrayLiteral("AUTHENTICATE ") + m_saslData.ChosenMechanism().toUtf8();
                if (m_caps.contains(QStringLiteral("SASL-IR"))) {
                    // we can send the response data now
                    m_response += " " + m_saslData.Response().toBase64();
                }
            } else if (m_protocol == QStringLiteral("smtp")) {
                if (m_saslData.Response().isEmpty()) {
                    m_response = QByteArrayLiteral("AUTH ") + m_saslData.ChosenMechanism().toUtf8();
                } else {
                    m_response = m_saslData.Response().toBase64();
                }
            }
            m_initialResponseSent = true;
        } else {
            m_response = m_saslData.Response().toBase64();
        }
    }
    qDebug() << "SASL DATA RESPONSE: " << m_response;
    m_responseReady = true;
    emit response(m_response);
}

void UOASaslPlugin::onErrorReceived(const SignOn::Error &error)
{
    m_hasError = true;
    m_responseReady = true;
    qWarning() << error.message();
}

void UOASaslPlugin::onPluginReset()
{
    m_initialResponseSent = false;
}
