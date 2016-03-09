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
#ifndef AUTOCONFIG_H
#define AUTOCONFIG_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPointer>
#include <ubuntu/connectivity/networking-status.h>
#include "serverconfiguration.h"

using ubuntu::connectivity::NetworkingStatus;
class AutoDiscover;
class AutoConfig : public QObject
{
    Q_OBJECT
public:
    AutoConfig(QObject *parent, ServerConfiguration *config);

public slots:
    void lookUp(const QUrl &url);
    bool networkAccessible() const;

signals:
    void success(ServerConfiguration *serverConfig);
    void failed();

private slots:
    void handleRequestResponse(QNetworkReply *reply);

private:
    QPointer<ServerConfiguration> m_config;
    QPointer<QNetworkAccessManager> m_nam;
    QScopedPointer<NetworkingStatus> m_netStatus;
    bool parseResponse(const QByteArray &response);
    bool configFromResponse(const QByteArray &response, const QString &tagName, const QString &protocol);

    // For unit tests
    friend class AutoDiscover;
    void fakeLookUp(const QUrl &url);

};

#endif // AUTOCONFIG_H

