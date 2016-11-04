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
#ifndef SRVLOOKUP_H
#define SRVLOOKUP_H

#include <QObject>
#include <QDnsLookup>
#include <QDnsServiceRecord>
#include <QPointer>
#include "serverconfiguration.h"

class SrvLookup : public QObject
{
    Q_OBJECT
public:
    SrvLookup(QObject *parent, ServerConfiguration *config);

    enum LookupType {
        INVALID,
        IMAPS,
        IMAP,
        SUBMISSION
    };

signals:
    void success(ServerConfiguration *serverConfig);
    void failed();

public slots:
    void lookUp(const QString &domain);

private slots:
    void buildNextRequest();
    void handleLookUpResult();
    void handleReceivedRecords();
    void addRecordToConfig(const QDnsServiceRecord &record);

private:
    QPointer<QDnsLookup> m_dns;
    QPointer<ServerConfiguration> m_serverConfig;
    LookupType m_lookupType;
    QString m_domain;
};

#endif // SRVLOOKUP_H
