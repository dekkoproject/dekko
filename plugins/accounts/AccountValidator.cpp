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
    m_retrievelAction = new QMailRetrievalAction(this);
    connect(m_retrievelAction, &QMailRetrievalAction::activityChanged, this, &AccountValidator::handleAccountActivity);
    m_transmitAction = new QMailTransmitAction(this);
    connect(m_transmitAction, &QMailTransmitAction::activityChanged, this, &AccountValidator::handleAccountActivity);

    if (m_account->accountId().isValid()) {
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
        m_timer->start(60 * 1000);
        m_retrievelAction->retrieveFolderList(m_account->accountId(), QMailFolderId(), true);
        m_state = RetrieveFolderList;
    } else {
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
                m_retrievelAction->createStandardFolders(m_account->accountId());
                m_state = CreateStandardFolders;
                return;
            }
            case CreateStandardFolders:
            {
                // Incomings good. finally test outgoing
                m_transmitAction->transmitMessages(m_account->accountId());
                m_state = TransmitMessage;
                return;
            }
            case None:
            case TransmitMessage:
                Q_UNREACHABLE();
            }
        } else if (activity == QMailServiceAction::Failed) {
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
    m_timer->stop();
    switch (status.errorCode) {
    case QMailServiceAction::Status::ErrFrameworkFault:
    case QMailServiceAction::Status::ErrSystemError:
    case QMailServiceAction::Status::ErrInternalServer:
    case QMailServiceAction::Status::ErrEnqueueFailed:
    case QMailServiceAction::Status::ErrInternalStateReset:
        emit failed(serviceType, InternalError);
        break;
    case QMailServiceAction::Status::ErrLoginFailed:
        emit failed(serviceType, FailedLogin);
        break;
    case QMailServiceAction::Status::ErrFileSystemFull:
        emit failed(serviceType, DiskFull);
        break;
    case QMailServiceAction::Status::ErrUnknownResponse:
        emit failed(serviceType, ExternalError);
        break;
    case QMailServiceAction::Status::ErrNoConnection:
    case QMailServiceAction::Status::ErrConnectionInUse:
    case QMailServiceAction::Status::ErrConnectionNotReady:
        emit failed(serviceType, ConnectionError);
        break;
    case QMailServiceAction::Status::ErrConfiguration:
    case QMailServiceAction::Status::ErrInvalidAddress:
    case QMailServiceAction::Status::ErrInvalidData:
    case QMailServiceAction::Status::ErrNotImplemented:
    case QMailServiceAction::Status::ErrNoSslSupport:
        emit failed(serviceType, ConfigInvalid);
        break;
    case QMailServiceAction::Status::ErrTimeout:
        emit failed(serviceType, Timeout);
        break;
    case QMailServiceAction::Status::ErrUntrustedCertificates:
        emit failed(serviceType, UntrustedCertificates);
        break;
    case QMailServiceAction::Status::ErrCancel:
        // The operation was cancelled by user intervention.
        break;
    default:
        emit failed(serviceType, InternalError);
        break;
    }
    setInProgress(false);
    cleanUp();
}

void AccountValidator::cleanUp()
{
    m_retrievelAction->deleteLater();
    m_transmitAction->deleteLater();
}

