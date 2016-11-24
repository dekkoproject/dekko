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
#ifndef UOABEARERPLUGIN_H
#define UOABEARERPLUGIN_H

#include <QObject>
#include <qmailaccountconfiguration.h>
#include <UOAService.h>
#include <signon-plugins/oauth2data.h>

class UOABearerPlugin : public UOAService
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "dekko.dekkoproject.uoa.OAuthBearerPlugin")
public:
    explicit UOABearerPlugin(QObject *parent = 0);
    // UOAService interface
public:
    virtual QString name() const override;
    virtual UOAService *create(const quint32 &uoaId) override;
    // call this only for the initial client response all further processing
    // should call processServerResponse
    virtual QByteArray process(const SignOn::SessionData &data, const QString &ua);
    // call when we receive a continuation response from the server
    virtual QByteArray processServerResponse(const QByteArray &response);
protected slots:
    virtual void onResponseReceived(const SignOn::SessionData &data) override;
    virtual void onErrorReceived(const SignOn::Error &error) override;
    virtual void onPluginReset() override;

private:
    QByteArray getMechFromCaps();
    QByteArray buildTokenResponse();
    OAuth2PluginNS::OAuth2PluginData m_data;
    OAuth2PluginNS::OAuth2PluginTokenData m_tokenData;
    bool m_initialResponseSent;
};

#endif // UOABEARERPLUGIN_H
