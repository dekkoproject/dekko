/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "smtpservice.h"
#ifndef QMF_NO_MESSAGE_SERVICE_EDITOR
#include "smtpsettings.h"
#endif
#include <QtPlugin>
#include <QTimer>
#include <QCoreApplication>
#include <qmaillog.h>
#include <QNetworkConfigurationManager>

namespace { const QString serviceKey("smtp"); }

class SmtpService::Sink : public QMailMessageSink
{
    Q_OBJECT

public:
    Sink(SmtpService *service) 
        : QMailMessageSink(service),
          _service(service)
    {
        connect(&_service->_client, SIGNAL(messageTransmitted(QMailMessageId)), this, SLOT(messageTransmitted(QMailMessageId)));
        connect(&_service->_client, SIGNAL(sendCompleted()), this, SLOT(sendCompleted()));
    }

public slots:
    virtual bool transmitMessages(const QMailMessageIdList &ids);

    void messageTransmitted(const QMailMessageId &id);
    void sendCompleted();

private:
    SmtpService *_service;
};

bool SmtpService::Sink::transmitMessages(const QMailMessageIdList &ids)
{
    QMailMessageIdList failedMessages;

    if (!ids.isEmpty()) {
        foreach (const QMailMessageId id, ids) {
            QMailMessage message(id);
            if (!(_service->_client.addMail(message) == QMailServiceAction::Status::ErrNoError)) {
                failedMessages << id;
            }
        }
    }

    if (failedMessages.count()) {
        emit messagesFailedTransmission(failedMessages, QMailServiceAction::Status::ErrInvalidAddress);
    }

    // Open new connection, even if there's no messages queued for transmission, client might
    // want to test the connection or the smtp server capabilities
    // (e.g. forward without download capable) are not known
     _service->_client.newConnection();
     return true;
}

void SmtpService::Sink::messageTransmitted(const QMailMessageId &id)
{
    emit messagesTransmitted(QMailMessageIdList() << id);
}

void SmtpService::Sink::sendCompleted()
{
    emit _service->actionCompleted(true);
}


SmtpService::SmtpService(const QMailAccountId &accountId)
    : QMailMessageService(),
      _client(this),
      _sink(new Sink(this)),
      _capabilityFetchAction(0),
      _capabilityFetchTimeout(0),
      _networkManager(0)
{
    connect(&_client, SIGNAL(progressChanged(uint, uint)), this, SIGNAL(progressChanged(uint, uint)));

    connect(&_client, SIGNAL(errorOccurred(int, QString)), this, SLOT(errorOccurred(int, QString)));
    connect(&_client, SIGNAL(errorOccurred(QMailServiceAction::Status, QString)), this, SLOT(errorOccurred(QMailServiceAction::Status, QString)));
    connect(&_client, SIGNAL(updateStatus(QString)), this, SLOT(updateStatus(QString)));

    _client.setAccount(accountId);

// Don't try to autoconnect on account creation for SSO services
#ifndef USE_ACCOUNTS_QT
    fetchCapabilities();
#endif
}

void SmtpService::fetchCapabilities()
{
    QMailAccount account(_client.account());
    if (account.customField("qmf-smtp-capabilities-listed") != "true") {
        // This will fetch the account capabilities from the server.
        QMailMessageKey accountKey(QMailMessageKey::parentAccountId(_client.account()));
        QMailMessageKey outboxKey(QMailMessageKey::status(QMailMessage::Outbox) & ~QMailMessageKey::status(QMailMessage::Trash));
        QMailMessageKey sendKey(QMailMessageKey::customField("dontSend", "true", QMailDataComparator::NotEqual));
        QMailMessageKey noSendKey(QMailMessageKey::customField("dontSend", QMailDataComparator::Absent));
        QMailMessageIdList toTransmit(
                    QMailStore::instance()->queryMessages(
                        accountKey & outboxKey & (noSendKey | sendKey)));
        if (toTransmit.isEmpty()) {
            // Only if there are no messages in Outbox!
            // Create a new action. It is deleted in the slot.
            qMailLog(SMTP) << "Fetching capabilities from the server...";
            if (!_capabilityFetchAction) {
                _capabilityFetchAction = new QMailTransmitAction(this);
                connect(_capabilityFetchAction, SIGNAL(activityChanged(QMailServiceAction::Activity)),
                        this, SLOT(onCapabilityFetchingActivityChanged(QMailServiceAction::Activity)));
            }
            _capabilityFetchAction->transmitMessages(_client.account());
        }
    }
}

void SmtpService::onCapabilityFetchingActivityChanged(QMailServiceAction::Activity activity)
{
    Q_ASSERT(_capabilityFetchAction);

    if (activity != QMailServiceAction::Successful
        && activity != QMailServiceAction::Failed) {
        return;
    }

    // Check for success.
    QMailAccount account(_client.account());
    if (account.customField("qmf-smtp-capabilities-listed") == "true") {
        if (_capabilityFetchTimeout) {
            delete _capabilityFetchTimeout;
            _capabilityFetchTimeout = 0;
        }
#ifdef USE_CONNECTIVITY_API
        if (!_networkManager.isNull()) {
            _networkManager.clear();
        }
#else
        if (_networkManager) {
            delete _networkManager;
            _networkManager = 0;
        }
#endif
        _capabilityFetchAction->deleteLater();
        _capabilityFetchAction = 0;
        return;
    }

    // The capabilities are not fetched yet. We
    // have to schedule another request.
#ifdef USE_CONNECTIVITY_API
    if (_networkManager.isNull()) {
        _networkManager = QSharedPointer<Connectivity>(new Connectivity);
        connect(_networkManager.data(), &Connectivity::onlineUpdated,
                this, &SmtpService::onOnlineStateChanged);
    }
#else
    if (!_networkManager) {
        _networkManager = new QNetworkConfigurationManager(this);
        connect(_networkManager, SIGNAL(onlineStateChanged(bool)),
                this, SLOT(onOnlineStateChanged(bool)));
    }
#endif
#ifdef USE_CONNECTIVITY_API
    if (_networkManager->online()) {
#else
    if (_networkManager->isOnline()) {
#endif
        // We are online. It makes sense to try again.
        uint capabilityCheckTimeoutLimit = 5 * 60 * 1000; // 5 minutes
        uint timeout = 1000;
        if (!_capabilityFetchTimeout) {
            _capabilityFetchTimeout = new QTimer(this);
            _capabilityFetchTimeout->setSingleShot(true);
            connect(_capabilityFetchTimeout, SIGNAL(timeout()),
                    this, SLOT(fetchCapabilities()));
        }
        else {
            timeout = _capabilityFetchTimeout->interval() << 2; // * 4
        }
        if (timeout <= capabilityCheckTimeoutLimit) {
            qMailLog(SMTP) << "Could not fetch capabilities...trying again after " << (timeout / 1000) << "seconds";
            _capabilityFetchTimeout->setInterval(timeout);
            _capabilityFetchTimeout->start();
        }
        else {
            qMailLog(SMTP) << "Could not fetch capabilities."
                           << "Disconnect and reconnect the network connection or"
                           << "update the account to try again";
            connect(QMailStore::instance(), SIGNAL(accountsUpdated(QMailAccountIdList)),
                    this, SLOT(onAccountsUpdated(QMailAccountIdList)));
        }
    }
}

void SmtpService::onOnlineStateChanged(bool isOnline)
{
    Q_ASSERT(_capabilityFetchAction);
    if (!isOnline
        || _capabilityFetchAction->activity() == QMailServiceAction::InProgress) {
        return;
    }
    if (_capabilityFetchTimeout) {
        if (_capabilityFetchTimeout->isActive()) {
            _capabilityFetchTimeout->stop();
        }
        _capabilityFetchTimeout->setInterval(1000);
    }
    fetchCapabilities();
}

void SmtpService::onAccountsUpdated(const QMailAccountIdList &accountIds)
{
    Q_ASSERT(_capabilityFetchAction);
    Q_ASSERT(_networkManager);
    Q_ASSERT(_capabilityFetchTimeout);
#ifdef USE_CONNECTIVITY_API
    if (!accountIds.contains(_client.account())
        || !_networkManager->online()
        || _capabilityFetchAction->activity() == QMailServiceAction::InProgress) {
        return;
    }
#else
    if (!accountIds.contains(_client.account())
        || !_networkManager->isOnline()
        || _capabilityFetchAction->activity() == QMailServiceAction::InProgress) {
        return;
    }
#endif
    disconnect(QMailStore::instance(), SIGNAL(accountsUpdated(QMailAccountIdList)),
               this, SLOT(onAccountsUpdated(QMailAccountIdList)));
    if (_capabilityFetchTimeout) {
        if (_capabilityFetchTimeout->isActive()) {
            _capabilityFetchTimeout->stop();
        }
        _capabilityFetchTimeout->setInterval(1000);
    }
    fetchCapabilities();
}

SmtpService::~SmtpService()
{
#ifdef USE_ACCOUNTS_QT
    //If account was deleted, we should remove appropriate signon identity from the signon.db
    if (!accountId().isValid())
        _client.removeSsoIdentity(accountId());
#endif
    delete _sink;
}

QString SmtpService::service() const
{
    return serviceKey;
}

QMailAccountId SmtpService::accountId() const
{
    return _client.account();
}

bool SmtpService::hasSink() const
{
    return true;
}

QMailMessageSink &SmtpService::sink() const
{
    return *_sink;
}

bool SmtpService::available() const
{
    return true;
}

bool SmtpService::cancelOperation(QMailServiceAction::Status::ErrorCode code, const QString &text)
{
    _client.cancelTransfer(code, text);
    return true;
}

void SmtpService::errorOccurred(int code, const QString &text)
{
    updateStatus(code, text, _client.account());
    emit actionCompleted(false);
}

void SmtpService::errorOccurred(const QMailServiceAction::Status & status, const QString &text)
{
    updateStatus(status.errorCode, text, _client.account(), status.folderId, status.messageId);
    emit actionCompleted(false);
}

void SmtpService::updateStatus(const QString &text)
{
    updateStatus(QMailServiceAction::Status::ErrNoError, text, _client.account());
}


class SmtpConfigurator : public QMailMessageServiceConfigurator
{
public:
    SmtpConfigurator();
    ~SmtpConfigurator();

    virtual QString service() const;
    virtual QString displayName() const;

#ifndef QMF_NO_MESSAGE_SERVICE_EDITOR
    virtual QMailMessageServiceEditor *createEditor(QMailMessageServiceFactory::ServiceType type);
#endif
};

SmtpConfigurator::SmtpConfigurator()
{
}

SmtpConfigurator::~SmtpConfigurator()
{
}

QString SmtpConfigurator::service() const
{
    return serviceKey;
}

QString SmtpConfigurator::displayName() const
{
    return QCoreApplication::instance()->translate("QMailMessageService", "SMTP");
}

#ifndef QMF_NO_MESSAGE_SERVICE_EDITOR
QMailMessageServiceEditor *SmtpConfigurator::createEditor(QMailMessageServiceFactory::ServiceType type)
{
    if (type == QMailMessageServiceFactory::Sink)
        return new SmtpSettings;

    return 0;
}
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(smtp,SmtpServicePlugin);
#endif

SmtpServicePlugin::SmtpServicePlugin()
    : QMailMessageServicePlugin()
{
}

QString SmtpServicePlugin::key() const
{
    return serviceKey;
}

bool SmtpServicePlugin::supports(QMailMessageServiceFactory::ServiceType type) const
{
    return (type == QMailMessageServiceFactory::Sink);
}

bool SmtpServicePlugin::supports(QMailMessage::MessageType type) const
{
    return (type == QMailMessage::Email);
}

QMailMessageService *SmtpServicePlugin::createService(const QMailAccountId &id)
{
    return new SmtpService(id);
}

QMailMessageServiceConfigurator *SmtpServicePlugin::createServiceConfigurator()
{
    return new SmtpConfigurator();
}

#include "smtpservice.moc"

