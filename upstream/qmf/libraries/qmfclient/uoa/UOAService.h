#ifndef UOASERVICE_H
#define UOASERVICE_H

#include "qmailglobal.h"
#include <QtPlugin>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QPointer>
#include <Accounts/Account>
#include <Accounts/AccountService>
#include <Accounts/AuthData>
#include <SignOn/AuthSession>
#include <SignOn/Identity>
#include <SignOn/SessionData>
#include "qmailaccount.h"
#include "qmailaccountconfiguration.h"
#include "UOAManager.h"

class QMF_EXPORT UOAService : public QObject
{
    Q_OBJECT
public:
    explicit UOAService(QObject *parent = 0);
    ~UOAService();

    virtual QString name() const = 0;
    virtual UOAService *create(const quint32 &uoaId) = 0;

    virtual QByteArray process(const SignOn::SessionData &data, const QString &mech = QString());
    virtual QByteArray processServerResponse(const QByteArray &response) = 0;
    SignOn::SessionData sessionData() const;
    void setCapabilities(const QStringList &caps) {m_caps = caps;}
    void setProtocol(const QString &protocol) { m_protocol = protocol; }
    void setServiceConfig(const QMailAccountConfiguration::ServiceConfiguration &cfg) { m_config = cfg; }
signals:
    void response(const QByteArray &response);
    void error();

protected slots:
    virtual void onResponseReceived(const SignOn::SessionData &data);
    virtual void onErrorReceived(const SignOn::Error &error);
    virtual void onPluginReset(){}

protected:
    void setUOAAccountId(const quint32 &id);
    void setSessionType(const QString &sessionType);
    QMailAccountId m_id;
    quint32 m_uoaId;
    QPointer<Accounts::Account> m_account;
    Accounts::AccountService *m_service;
    QPointer<SignOn::AuthSession> m_session;
    SignOn::Identity *m_identity;
    SignOn::SessionData m_sessionData;
    QStringList m_caps;
    QString m_sessionType;
    bool m_responseReady;
    bool m_hasError;
    QByteArray m_response;
    QString m_protocol;
    QMailAccountConfiguration::ServiceConfiguration m_config;

private:
    void init();
};

#endif // UOASERVICE_H
