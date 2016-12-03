#include "UOAService.h"
#include <QCoreApplication>
#include <QDebug>

UOAService::UOAService(QObject *parent) : QObject(parent),
    m_account(0), m_service(0), m_session(0), m_identity(0), m_responseReady(false), m_hasError(false)
{
}

UOAService::~UOAService()
{
    m_account->sync();
}

QByteArray UOAService::process(const SignOn::SessionData &data, const QString &mech)
{
    if (m_session.isNull()) {
        qWarning() << "No usable session";
        return QByteArray();
    }
    m_responseReady = false;
    m_hasError = false;
    m_session->process(data, mech);

    while (!m_responseReady) {
        // Process events while we wait
        QCoreApplication::processEvents();
    }

    if (m_hasError || m_response.isEmpty()) {
        return QByteArray();
    }
    return m_response;
}

SignOn::SessionData UOAService::sessionData() const
{
    return SignOn::SessionData(m_service->authData().parameters());
}

void UOAService::onResponseReceived(const SignOn::SessionData &data)
{
    qWarning() << "Not implemented here. Session data not handled";
}

void UOAService::onErrorReceived(const SignOn::Error &error)
{
    qWarning() << "Not implemented here. Session error not handled";
}

void UOAService::setUOAAccountId(const quint32 &id)
{
    m_uoaId = id;
    if (m_account.isNull()) {
        init();
        return;
    }
    onPluginReset();
    delete m_account.data();
    m_account.clear();
    delete m_service;
    m_service = 0;
    delete m_identity;
    m_identity = 0;
    m_session->disconnect();
    m_session.clear();
    init();
}

void UOAService::init()
{
    m_account = UOASharedManager::instance()->account((Accounts::AccountId)m_uoaId);
    m_service = new Accounts::AccountService(m_account.data(), Accounts::Service(), this);
    m_identity = SignOn::Identity::existingIdentity(m_account->credentialsId(), this);
    if (m_identity) {
        if (m_sessionType.isEmpty()) {
            return;
        }
        m_session = m_identity->createSession(m_sessionType);
        if (m_session.isNull()) {
            qFatal("[UOAService] Invalid session");
        }
        connect(m_session, &SignOn::AuthSession::response, this, &UOAService::onResponseReceived);
        connect(m_session, &SignOn::AuthSession::error, this, &UOAService::onErrorReceived);
    }
}
