/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
*/
#include "AccountValidator.h"

AccountValidator::AccountValidator(QObject *parent) : QObject(parent),
    m_inProgress(false), m_state(None), m_timer(new QTimer(this))
{
}

void AccountValidator::validateAccount(Account *account)
{
    if (m_inProgress) {
        emit failed(AccountConfiguration::IMAP, ValidationAlreadyInProgress);
        return;
    }
    setInProgress(true);
    m_account = account;
    init();
    if (m_account->accountId().isValid()) {
        m_timer->start(60 * 1000);
        m_retrievelAction->retrieveFolderList(m_account->accountId(), QMailFolderId(), true);
        m_state = RetrieveFolderList;
    } else {
        emit validationFailed();
        emit failed(static_cast<AccountConfiguration *>(m_account->incoming())->serviceType(), AccountInvalid);
    }
}

void AccountValidator::handleAccountActivity(QMailServiceAction::Activity activity)
{
    if (static_cast<QObject *>(m_retrievelAction) == sender()) {
        if (activity == QMailServiceAction::Successful) {
            switch(m_state) {
            case RetrieveFolderList:
            {
                // Success retrieving folders so create the standard folders
                m_state = CreateStandardFolders;
                m_retrievelAction->createStandardFolders(m_account->accountId());
                break;
            }
            case CreateStandardFolders:
            {
                // Incomings good
                m_state = TransmitMessage;
                m_transmitAction->transmitMessages(m_account->accountId());
                break;
            }
            case TransmitMessage:
            {
                break;
            }
            case None:
                break;
            }
        } else if (activity == QMailServiceAction::Failed) {
            qDebug() << __func__ << "FAILED";
            AccountConfiguration *incoming = static_cast<AccountConfiguration *>(m_account->incoming());
            testFailed(incoming->serviceType(), m_retrievelAction->status());
        }
    } else if (static_cast<QObject *>(m_transmitAction) == sender()) {
        if (activity == QMailServiceAction::Successful) {
            if (m_state == TransmitMessage) {
                // Yay!!!
                m_timer->stop();
                emit success();
                setInProgress(false);
                cleanUp();
                return;
            }
        } else if (activity == QMailServiceAction::Failed) {
            AccountConfiguration *outgoing = static_cast<AccountConfiguration *>(m_account->outgoing());
            testFailed(outgoing->serviceType(), m_retrievelAction->status());
        }
    }
}

void AccountValidator::testFailed(AccountConfiguration::ServiceType serviceType, QMailServiceAction::Status status)
{

    if (!m_inProgress) {
        return;
    }
    setInProgress(false);
    m_timer->stop();
    emit validationFailed();
    emit failedActionStatus(status);
    cleanUp();
}

void AccountValidator::init()
{
    m_retrievelAction = new QMailRetrievalAction(this);
    connect(m_retrievelAction, &QMailRetrievalAction::activityChanged, this, &AccountValidator::handleAccountActivity);
    m_transmitAction = new QMailTransmitAction(this);
    connect(m_transmitAction, &QMailTransmitAction::activityChanged, this, &AccountValidator::handleAccountActivity);
    connect(m_timer, &QTimer::timeout, [=]() {
        m_timer->stop();
          if (m_retrievelAction->isRunning()) {
              m_retrievelAction->cancelOperation();
          }
          if (m_transmitAction->isRunning()) {
              m_transmitAction->cancelOperation();
          }
          AccountConfiguration *conf = 0;
          if (m_state == TransmitMessage) {
                conf = static_cast<AccountConfiguration *>(m_account->outgoing());
          } else {
              conf = static_cast<AccountConfiguration *>(m_account->incoming());
          }
          emit failed(conf->serviceType(), Timeout);
          setInProgress(false);
          cleanUp();
    });
}

void AccountValidator::cleanUp()
{
    m_retrievelAction->deleteLater();
    m_transmitAction->deleteLater();
}

