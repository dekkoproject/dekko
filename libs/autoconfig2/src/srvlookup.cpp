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
#include "srvlookup.h"
#include <QStringBuilder>
#include <QDebug>

SrvLookup::SrvLookup(QObject *parent, ServerConfiguration *config) :
    QObject(parent), m_serverConfig(config), m_dns(new QDnsLookup), m_lookupType(INVALID)
{
    connect(m_dns, &QDnsLookup::finished, this, &SrvLookup::handleLookUpResult);
}

void SrvLookup::lookUp(const QString &domain)
{
    if (domain.isEmpty()) {
        emit failed();
        return;
    }
    m_dns->setType(QDnsLookup::SRV);
    m_domain = domain;
    // Ideally we always prefer IMAPS over IMAP even if it
    // may get upgraded via STARTTLS. So start with this first.
    m_lookupType = IMAPS;
    buildNextRequest();
}

void SrvLookup::buildNextRequest()
{
    switch (m_lookupType) {
    case INVALID:
        // Shouldn't be here
        emit failed();
        return;
    case IMAPS:
        m_dns->setName(QString("_imaps._tcp." % m_domain));
        m_dns->lookup();
        break;
    case IMAP:
        m_dns->setName(QString("_imap._tcp." % m_domain));
        m_dns->lookup();
        break;
    case SUBMISSION:
        m_dns->setName(QString("_submission._tcp." % m_domain));
        m_dns->lookup();
        break;
    }
    qDebug() << "Looking for SRV record at: " << m_dns->name();
}

void SrvLookup::handleLookUpResult()
{
    if (m_dns->error() != QDnsLookup::NoError) {
        qDebug() << "SRV lookup failed: " << m_dns->errorString();
        switch (m_lookupType) {
        case INVALID:
            // Shouldn't be here
            emit failed();
            return;
        case IMAPS:
            m_lookupType = IMAP;
            buildNextRequest();
            return;
        case IMAP:
            // If we reached here then we failed to find any IMAP info
            // So we need to just fail as we don't currently configure
            // SMTP without IMAP info.
        case SUBMISSION:
            emit failed();
            return;
        }
    }
    // Now deal with the received records
    handleReceivedRecords();

    // Determine the next step.
    switch(m_lookupType) {
    case INVALID:
        emit failed();
        return;
    case IMAPS:
    case IMAP:
        // We found details for IMAP so we want to jump to submission now
        m_lookupType = SUBMISSION;
        buildNextRequest();
        break;
    case SUBMISSION:
        // Were complete now
        if (m_serverConfig->isValid()) {
            emit success(m_serverConfig);
        } else {
            qWarning("Config isn't valid");
            emit failed();
        }
        break;
    }
}

void SrvLookup::handleReceivedRecords()
{
    if (m_dns->serviceRecords().size() == 1) {
        addRecordToConfig(m_dns->serviceRecords().at(0));
    } else {
        // rfc6186 section 3.4 states the record with the lowest priority value
        // Is the one we should use.
        int i = 0;
        int idx = -1;
        quint16 priority = -1;

        auto updateInterestingRecord = [&](const int &index, const quint16 &recordPriority) {
            idx = index;
            priority = recordPriority;
        };

        Q_FOREACH (const QDnsServiceRecord &record, m_dns->serviceRecords()) {
            if (priority == -1) {
                //this is first in the list so is the potential candidate
                updateInterestingRecord(i, record.priority());
            } else {
                if (record.priority() < priority) {
                    updateInterestingRecord(i, record.priority());
                }
            }
            ++i;
        }
        addRecordToConfig(m_dns->serviceRecords().at(idx));
    }
}


void SrvLookup::addRecordToConfig(const QDnsServiceRecord &record)
{
    // Sort host & port first
    switch (m_lookupType) {
    case INVALID:
        emit failed();
        return;
    case IMAPS:
    case IMAP:
        m_serverConfig->setImapHost(record.target());
        m_serverConfig->setImapPort(record.port());
        if (m_lookupType == IMAPS) {
            m_serverConfig->setImapMethod(ServerConfiguration::SSL_TLS);
        } else {
            // We assume STARTLS as it's very uncommon that it would
            // be cleartext and a client usually decide that on the fact of the LOGINDISABLED
            // capability being present
            m_serverConfig->setImapMethod(ServerConfiguration::STARTTLS);
        }
    case SUBMISSION:
        m_serverConfig->setSmtpHost(record.target());
        m_serverConfig->setSmtpPort(record.port());
        if (record.port() == 465) {
            m_serverConfig->setSmtpMethod(ServerConfiguration::SSL_TLS);
        } else {
            m_serverConfig->setSmtpMethod(ServerConfiguration::STARTTLS);
        }
    }
}
