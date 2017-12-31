/* Copyright (C) 2016 - 2017 Dan Chapman <dpniel@ubuntu.com>

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
#ifndef ACCOUNTVALIDATOR_H
#define ACCOUNTVALIDATOR_H

#include <QLoggingCategory>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <qmailserviceaction.h>
#include "Account.h"

Q_DECLARE_LOGGING_CATEGORY(D_ACCOUNTS_VALIDATOR)

/** @ingroup group_accounts */
class AccountValidator: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool inProgress READ inProgress NOTIFY inProgressChanged)
    Q_ENUMS(FailureReason)
    Q_ENUMS(State)
public:
    explicit AccountValidator(QObject *parent = 0);

    enum FailureReason {
        AccountInvalid,
        ConfigInvalid,
        ConnectionError,
        DiskFull,
        ExternalError,
        FailedLogin,
        InternalError,
        Timeout,
        UntrustedCertificates,
        ValidationAlreadyInProgress
    };

    enum State {
        None,
        RetrieveFolderList,
        CreateStandardFolders,
        TransmitMessage
    };
    bool inProgress() const { return m_inProgress; }

signals:
    void success();
    void failed(AccountConfiguration::ServiceType service, FailureReason reason);
    void failedActionStatus(QMailServiceAction::Status status);
    void validationFailed(); // qml one
    void inProgressChanged();

public slots:
    void validateAccount(Account *account);

private slots:
    void handleAccountActivity(QMailServiceAction::Activity activity);

    void createStandardFolders();
    void testTransmission();

private:
    QPointer<Account> m_account; // store it locally
    QPointer<QMailRetrievalAction> m_retrievelAction;
    QPointer<QMailTransmitAction> m_transmitAction;
    bool m_inProgress;
    State m_state;
    QTimer *m_timer;

    void testFailed(AccountConfiguration::ServiceType serviceType, QMailServiceAction::Status status);
    void init();
    void cleanUp();
    void setInProgress(const bool inProgress) {
        m_inProgress = inProgress;
        emit inProgressChanged();
    }
};

#endif // ACCOUNTVALIDATOR_H
