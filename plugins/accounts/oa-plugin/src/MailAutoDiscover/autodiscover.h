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
#ifndef AUTODISCOVER_H
#define AUTODISCOVER_H

#include <QObject>
#include <QPointer>
#include <QScopedPointer>
#include "autoconfig.h"
#include "emailvalidator.h"
#include "serverconfiguration.h"
#include "srvlookup.h"

class AutoDiscover : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool inProgress READ inProgress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_ENUMS(Status)
public:
    explicit AutoDiscover(QObject *parent = 0);

    enum Status {
        INVALID,
        NEW_REQUEST,
        REQUEST_AUTOCONFIG,
        REQUEST_AUTOCONFIG_WELLKNOWN,
        REQUEST_AUTOCONFIG_ISPDB,
        REQUEST_SRV,
//        REQUEST_WEBFINGER, // Is this possible?? it should be!
        REQUEST_FAILED,
        REQUEST_SUCCEEDED,
        BUILDING_RESULT
    };

    Q_INVOKABLE bool isValidMailAddress(const QString &address);
    bool inProgress() const;
    Status status() const;

signals:
    void invalidMailAddress(QString address);
    void success(ServerConfiguration *serverConfig) const;
    void failed();
    void progressChanged();
    void statusChanged();
    void noNetworkAvailable();

public slots:
    void lookUp(const QString &mailAddress);

private:
    void buildNextRequest();
    void handleRequestFailed();
    void setStatus(Status status);

private slots:
    void handleRequestSucceeded(ServerConfiguration *config);

private:
    QPointer<ServerConfiguration> m_serverConfig;
    QPointer<AutoConfig> m_autoConfig;
    QPointer<SrvLookup> m_srvLookup;

    QString m_domain;
    Status m_status;
    bool m_inProgress;

    // For UNIT TESTS
    Q_PROPERTY(bool testMode MEMBER m_testMode)
    Q_PROPERTY(ServerConfiguration *_serverConf MEMBER m_serverConfig)
    bool m_testMode;

};

#endif // AUTODISCOVER_H
