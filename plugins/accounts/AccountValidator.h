#ifndef ACCOUNTVALIDATOR_H
#define ACCOUNTVALIDATOR_H

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <qmailserviceaction.h>
#include "Account.h"

class AccountValidator: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool inProgress READ inProgress NOTIFY inProgressChanged)
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
    void inProgressChanged();

public slots:
    void validateAccount(Account *account);

private slots:
    void handleAccountActivity(QMailServiceAction::Activity activity);

private:
    QPointer<Account> m_account; // store it locally
    QPointer<QMailRetrievalAction> m_retrievelAction;
    QPointer<QMailTransmitAction> m_transmitAction;
    bool m_inProgress;
    State m_state;
    QTimer *m_timer;

    void testFailed(AccountConfiguration::ServiceType serviceType, QMailServiceAction::Status status);
    void cleanUp();
    void setInProgress(const bool inProgress) {
        m_inProgress = inProgress;
        emit inProgressChanged();
    }
};

#endif // ACCOUNTVALIDATOR_H
