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
    m_serverConfig = new EmailProvider(this);
    m_autoConfig = new AutoConfig(this, m_serverConfig);
    connect(m_autoConfig, &AutoConfig::failed, this, &AutoDiscover::handleRequestFailed);
    connect(m_autoConfig, &AutoConfig::success, this, &AutoDiscover::handleRequestSucceeded);
//    m_srvLookup = new SrvLookup(this, m_serverConfig);
//    connect(m_srvLookup, &SrvLookup::failed, this, &AutoDiscover::handleRequestFailed);
//    connect(m_srvLookup, &SrvLookup::success, this, &AutoDiscover::handleRequestSucceeded);
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
    // Although this is a valid email address, it may not
    // actually contain a domain. AutoDiscover works on the
    // domain being valid so needs to have at least 1 period
    // i.e .com .co.uk etc
    // So fail autoconfig now and request manual entry
    if (!m_domain.contains(QLatin1Char('.'))) {
        emit failed();
        return;
    }
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
        qDebug() << "[AutoDiscover::NEW_REQUEST]" << "Requesting local first";
        setStatus(REQUEST_LOCAL);
        buildNextRequest();
        break;
    case REQUEST_LOCAL:
        qDebug() << "[AutoDiscover::REQUEST_LOCAL]" << "Looking for local config";
        m_autoConfig->findLocal(m_domain);
        break;
    case REQUEST_AUTOCONFIG_V12:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_V12]" << "Looking for autoconfig v1.2";
        m_autoConfig->lookUp(QString("http://autoconfig.%1/mail/config-v1.2.xml").arg(m_domain));
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN_V12:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_WELLKNOWN_V12]" << "Looking for wellknow autocnfig v1.2";
        m_autoConfig->lookUp(QString("http://%1/.well-known/autoconfig/mail/config-v1.2.xml").arg(m_domain));
        break;
    case REQUEST_AUTOCONFIG_V11:
        qDebug() << "[AutoDiscover::NEW_REQUEST]" << "Looking for autoconfig v1.1";
        m_autoConfig->lookUp(QString("http://autoconfig.%1/mail/config-v1.1.xml").arg(m_domain));
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN_V11:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_WELLKNOWN_V11]" << "Looking for wellknow autocnfig v1.1";
        m_autoConfig->lookUp(QString("http://%1/.well-known/autoconfig/mail/config-v1.1.xml").arg(m_domain));
        break;
    case REQUEST_DEKKO_ISPDB:
        qDebug() << "[AutoDiscover::REQUEST_DEKKO_ISPDB]" << "Querying dekko's ispdb";
        m_autoConfig->lookUp(QString("http://autoconfig.dekkoproject.org/v1.2/%1").arg(m_domain));
        break;
    case REQUEST_AUTOCONFIG_ISPDB:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_ISPDB]" << "Last chance! trying thunderbirds ispdb...";
        m_autoConfig->lookUp(QString("https://autoconfig.thunderbird.net/v1.1/%1").arg(m_domain));
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

    switch(m_status) {
    case INVALID:
    case NEW_REQUEST:
        setStatus(REQUEST_FAILED);
        emit failed(); // Nothing we can do from here now.
        break;
    case REQUEST_LOCAL:
        qDebug() << "[AutoDiscover::REQUEST_LOCAL]" << "No local configs";
        setStatus(REQUEST_AUTOCONFIG_V12);
        buildNextRequest();
        break;
    case REQUEST_AUTOCONFIG_V12:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_V12]" << "Failed lookup";
        setStatus(REQUEST_AUTOCONFIG_WELLKNOWN_V12);
        buildNextRequest();
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN_V12:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_WELLKNOWN_V12]" << "Failed lookup";
        setStatus(REQUEST_AUTOCONFIG_V11);
        buildNextRequest();
        break;
    case REQUEST_AUTOCONFIG_V11:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_V11]" << "Failed lookup";
        setStatus(REQUEST_AUTOCONFIG_WELLKNOWN_V11);
        buildNextRequest();
        break;
    case REQUEST_AUTOCONFIG_WELLKNOWN_V11:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_WELLKNOWN_V11]" << "Failed lookup";
        setStatus(REQUEST_DEKKO_ISPDB);
        buildNextRequest();
        break;
    case REQUEST_DEKKO_ISPDB:
        qDebug() << "[AutoDiscover::REQUEST_DEKKO_ISPDB]" << "Failed lookup";
        setStatus(REQUEST_AUTOCONFIG_ISPDB);
        buildNextRequest();
        break;
    case REQUEST_AUTOCONFIG_ISPDB:
        qDebug() << "[AutoDiscover::REQUEST_AUTOCONFIG_ISPDB]" << "Failed lookup";
        setStatus(REQUEST_FAILED);
        buildNextRequest();
        break;
    case REQUEST_FAILED:
    case REQUEST_SUCCEEDED:
    case BUILDING_RESULT:
        setStatus(REQUEST_FAILED);
        buildNextRequest();
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

void AutoDiscover::handleRequestSucceeded(EmailProvider *config)
{
    qDebug() << "++++++++++++++++++++++++";
    qDebug() << "SUCCESS!";
    qDebug() << "VERSION: " << config->version();
    qDebug() << "DisplayName: " << config->displayName();
    qDebug() << "Short display name: " << config->shortName();
    qDebug() << "Total incoming: " << config->incoming()->size();
    qDebug() << "Incoming host" << config->incoming()->at(0)->hostname();
    qDebug() << "++++++++++++++++++++++++";
    if (config == NULL || !config->isValid()) {
        setStatus(INVALID);
        emit failed();
        return;
    }
    setStatus(REQUEST_SUCCEEDED);
    m_serverConfig = config;
    if (config != NULL) {
        emit success();
    } else {
        // errr how did this happen??
        qDebug() << "[AutoDiscover] Somehow the configs don't match, they are: ";
//        qDebug() << "this: " << m_serverConfig->toMap();
//        qDebug() << "other: " << config->toMap();
        setStatus(INVALID);
        emit failed();
        return;
    }
}
