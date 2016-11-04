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
#include "emailprovider.h"

#ifdef USE_CONNECTIVITY_API
#include <connectivityqt/connectivity.h>
using connectivityqt::Connectivity;
#endif

class AutoDiscover;
class AutoConfig : public QObject
{
    Q_OBJECT
public:
    AutoConfig(QObject *parent, EmailProvider *config);

public slots:
    void findLocal(const QString &domain);
    void lookUp(const QUrl &url);
    bool networkAccessible() const;

signals:
    void success(EmailProvider *serverConfig);
    void failed();

private slots:
    void handleRequestResponse(QNetworkReply *reply);

private:
#ifdef USE_CONNECTIVITY_API
    QScopedPointer<Connectivity> m_netStatus;
#endif
    QPointer<EmailProvider> m_config;
    QPointer<QNetworkAccessManager> m_nam;
    // For unit tests
    friend class AutoDiscover;
    void fakeLookUp(const QUrl &url);

};

#endif // AUTOCONFIG_H

