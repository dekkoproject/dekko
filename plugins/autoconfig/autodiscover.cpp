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
#include "autodiscover.h"
#include <QDebug>

AutoDiscover::AutoDiscover(QObject *parent) :
    QObject(parent), m_inProgress(false), m_testMode(false)
{
    m_serverConfig = new ServerConfiguration(this);
    m_autoConfig = new AutoConfig(this, m_serverConfig);
    connect(m_autoConfig, &AutoConfig::failed, this, &AutoDiscover::handleRequestFailed);
    connect(m_autoConfig, &AutoConfig::success, this, &AutoDiscover::handleRequestSucceeded);
    m_srvLookup = new SrvLookup(this, m_serverConfig);
    connect(m_srvLookup, &SrvLookup::failed, this, &AutoDiscover::handleRequestFailed);
    connect(m_srvLookup, &SrvLookup::success, this, &AutoDiscover::handleRequestSucceeded);
    m_status = INVALID;
}

bool AutoDiscover::isValidMailAddress(const QString &address)
{
    return EmailValidator::instance()->validate(address);
}

bool AutoDiscover::inProgress() const
{
    return m_inProgress;
}

AutoDiscover::Status AutoDiscover::status() const
{
    return m_status;
}

void AutoDiscover::lookUp(const QString &mailAddress)
{
    if (!isValidMailAddress(mailAddress)) {
        emit invalidMailAddress(mailAddress);
        emit failed();
        return;
    }
    if (!mailAddress.contains("@", Qt::CaseInsensitive)) {
        emit failed();
        return;
    }
    m_domain = mailAddress.split("@")[1];
    setStatus(NEW_REQUEST);
    buildNextRequest();
}

void AutoDiscover::buildNextRequest()
{
    if (!m_testMode && !m_autoConfig->networkAccessible()) {
        emit noNetworkAvailable();
        emit failed();
        return;
    }
    switch(m_status) {
    case INVALID:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    case NEW_REQUEST:
        setStatus(REQUEST_AUTOCONFIG);
        buildNextRequest();
        break;
    case REQUEST_AUTOCONFIG:
        if (m_testMode) {
            m_autoConfig->fakeLookUp(QString("http://fakeurl.com"));
        } else {
            m_autoConfig->lookUp(QString("http://autoconfig.%1/mail/config-v1.1.xml").arg(m_domain));
        }
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN:
        m_autoConfig->lookUp(QString("http://%1/.well-known/autoconfig/mail/config-v1.1.xml").arg(m_domain));
        break;
    case REQUEST_AUTOCONFIG_ISPDB:
        m_autoConfig->lookUp(QString("https://autoconfig.thunderbird.net/v1.1/%1").arg(m_domain));
        break;
    case REQUEST_SRV:
        m_srvLookup->lookUp(m_domain);
        break;
    case REQUEST_FAILED:
    case REQUEST_SUCCEEDED:
    case BUILDING_RESULT:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    }
}

void AutoDiscover::handleRequestFailed()
{
    switch (m_status) {
    case INVALID:
    case NEW_REQUEST:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    case REQUEST_AUTOCONFIG:
        setStatus(REQUEST_AUTOCONFIG_WELLKNOWN);
        buildNextRequest();
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN:
        setStatus(REQUEST_AUTOCONFIG_ISPDB);
        buildNextRequest();
        break;
    // If the request failed on REQUEST_AUTOCONFIG_ISPDB then we
    // have nothing left we can do so just *fail* this.
    case REQUEST_AUTOCONFIG_ISPDB:
        setStatus(REQUEST_SRV);
        buildNextRequest();
        break;
    case REQUEST_SRV:
    case REQUEST_FAILED:
    case REQUEST_SUCCEEDED:
    case BUILDING_RESULT:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    }
}

void AutoDiscover::setStatus(AutoDiscover::Status status)
{
    // Anything other than INVALID and REQUEST_FAILED means we are in progress.
    if (status == INVALID || status == REQUEST_FAILED || status == REQUEST_SUCCEEDED) {
        m_inProgress = false;
    } else if (!m_inProgress) {
        m_inProgress = true;
    }
    m_status = status;
    emit progressChanged();
    emit statusChanged();
}

void AutoDiscover::handleRequestSucceeded(ServerConfiguration *config)
{
    if (config == NULL || !config->isValid()) {
        setStatus(INVALID);
        emit failed();
        return;
    }
    setStatus(REQUEST_SUCCEEDED);
    if (config == m_serverConfig) {
        emit success(m_serverConfig);
    } else {
        // errr how did this happen??
        qDebug() << "[AutoDiscover] Somehow the configs don't match, they are: ";
        qDebug() << "this: " << m_serverConfig->toMap();
        qDebug() << "other: " << config->toMap();
        setStatus(INVALID);
        emit failed();
        return;
    }
}
