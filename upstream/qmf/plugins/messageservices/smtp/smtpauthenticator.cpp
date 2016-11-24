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

#include "smtpauthenticator.h"

#include <qmailauthenticator.h>
#include <qmailstore.h>
#include <qmaillog.h>

namespace {

QMap<QMailAccountId, QList<QByteArray> > gResponses;

#ifdef USE_ACCOUNTS_QT
QString authPassword;
QMail::SaslMechanism responseAuthType;
#endif
}

#ifdef USE_ACCOUNTS_QT
// Returns authentication type from more secure to less secure supported
static SmtpConfiguration::AuthType authFromCapabilities(const QStringList &capabilities, const QMap<QString, QList<QByteArray> > &ssoLogin)
{

    QStringList authCaps;
    foreach (QString const& capability, capabilities) {
        if (capability.startsWith("AUTH", Qt::CaseInsensitive)) {
            authCaps.append(capability.split(" ",QString::SkipEmptyParts));
        }
    }
    qMailLog(SMTP) << "Auths found: " << authCaps;

    if (authCaps.contains("CRAM-MD5", Qt::CaseInsensitive) && ssoLogin.contains("CRAM-MD5")) {
        qMailLog(SMTP) << "Returning auth CRAM-MD5";
        return SmtpConfiguration::Auth_CRAMMD5;
    } else if (authCaps.contains("LOGIN", Qt::CaseInsensitive) && !authCaps.contains("PLAIN", Qt::CaseInsensitive)
               && ssoLogin.contains("LOGIN")) {
        // LOGIN mechanism is obsolete, use it as last resort, see: http://www.iana.org/assignments/sasl-mechanisms/sasl-mechanisms.xhtml
        qMailLog(SMTP) <<  "Returning auth LOGIN";
        return SmtpConfiguration::Auth_LOGIN;
    } else if (ssoLogin.contains("PLAIN")) {
        qMailLog(SMTP) << "Returning auth PLAIN";
        return SmtpConfiguration::Auth_PLAIN;
    } else {
        qMailLog(SMTP) << "Returning auth NONE";
        return SmtpConfiguration::Auth_NONE;
    }
}

static QByteArray authenticationResponses(QList<QByteArray> &authList, const SmtpConfiguration::AuthType &authType, const QMailAccountId &id)
{
    QByteArray result;
    if(!authList.empty()) {
        result = authList.takeFirst();
        if (!authList.empty()) {
            if (authType == SmtpConfiguration::Auth_CRAMMD5) {
                authPassword = QString::fromLatin1(authList.takeFirst());
                responseAuthType = QMail::CramMd5Mechanism;
            } else {
                gResponses[id] = authList;
            }
        }
    } else {
        qMailLog(SMTP) << "Failed to get authentication for method" << authType << "in account id:" << id;
    }
    return result;
}

QByteArray SmtpAuthenticator::getAuthentication(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities, const QMap<QString, QList<QByteArray> > &ssoLogin)
{
    QByteArray result;
    SmtpConfiguration smtpCfg(svcCfg);
    QMailAccountId id(smtpCfg.id());
    SmtpConfiguration::AuthType authType = static_cast<SmtpConfiguration::AuthType>(smtpCfg.smtpAuthentication());

    // if we don't have auth yet, try to get it from the capabilities
    if (ssoLogin.size() > 1 && authType == SmtpConfiguration::Auth_NONE
            && smtpCfg.smtpAuthFromCapabilities()) {
        qMailLog(SMTP) << "Discovering authentication from capabilities for account id:" << id;
        authType = authFromCapabilities(capabilities, ssoLogin);;
        if (authType != SmtpConfiguration::Auth_NONE) {
            QMailAccount account(id);
            QMailAccountConfiguration accountConfig(id);
            QMailAccountConfiguration::ServiceConfiguration serviceConf(accountConfig.serviceConfiguration("smtp"));
            serviceConf.setValue("authentication",QString::number(authType));
            if (!QMailStore::instance()->updateAccount(&account, &accountConfig)) {
                qWarning() << "Unable to update account" << account.id() << "to auth type!!!!";
            }
        }
    }

#ifndef QT_NO_OPENSSL
    if (authType != SmtpConfiguration::Auth_NONE) {
        if (!ssoLogin.isEmpty()) {
            QList<QByteArray> auth;

            if (ssoLogin.size() == 1) {
                QList<QString> keys = ssoLogin.keys();
                auth = ssoLogin.value(keys.at(0));
                result = authenticationResponses(auth, authType, id);
                qMailLog(SMTP) << "Using authentication method " << keys.at(0)
                               << " for account id:" << id;
            } else {
                if (authType == SmtpConfiguration::Auth_CRAMMD5) {
                    auth = ssoLogin.value("CRAM-MD5");
                    result = authenticationResponses(auth, authType, id);
                } else if (authType == SmtpConfiguration::Auth_PLAIN) {
                    auth = ssoLogin.value("PLAIN");
                    result = authenticationResponses(auth, authType, id);
                } else if (authType == SmtpConfiguration::Auth_LOGIN) {
                    auth = ssoLogin.value("LOGIN");
                    result = authenticationResponses(auth, authType, id);
                }
            }
        } else {
            qMailLog(SMTP) << Q_FUNC_INFO << "SSO identity is not found for account id: "<< id
                           << ", returning empty authentication";
        }
    }
#endif
    return result;
}

#else
QByteArray SmtpAuthenticator::getAuthentication(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities)
{
    QByteArray result(QMailAuthenticator::getAuthentication(svcCfg, capabilities));
    if (!result.isEmpty())
        return result.prepend("AUTH ");

#ifndef QT_NO_OPENSSL
    SmtpConfiguration smtpCfg(svcCfg);
    if (smtpCfg.smtpAuthentication() != SmtpConfiguration::Auth_NONE) {
        QMailAccountId id(smtpCfg.id());
        QByteArray username(smtpCfg.smtpUsername().toUtf8());
        QByteArray password(smtpCfg.smtpPassword().toUtf8());

        if (smtpCfg.smtpAuthentication() == SmtpConfiguration::Auth_LOGIN) {
            result = QByteArray("LOGIN");
            gResponses[id] = (QList<QByteArray>() << username << password);
        } else if (smtpCfg.smtpAuthentication() == SmtpConfiguration::Auth_PLAIN) {
            result = QByteArray("PLAIN ") + QByteArray(username + '\0' + username + '\0' + password).toBase64();
            gResponses[id] = (QList<QByteArray>() << QByteArray(username + '\0' + username + '\0' + password));
        }
    }
#endif

    if (!result.isEmpty()) {
        result.prepend("AUTH ");
    }
    return result;
}
#endif

QByteArray SmtpAuthenticator::getResponse(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QByteArray &challenge)
{
    QByteArray result;

    QMap<QMailAccountId, QList<QByteArray> >::iterator it = gResponses.find(svcCfg.id());
    if (it != gResponses.end()) {
        QList<QByteArray> &responses = it.value();
        result = responses.takeFirst();

        if (responses.isEmpty())
            gResponses.erase(it);
    } else {
#ifdef USE_ACCOUNTS_QT
        if (!authPassword.isEmpty()) {
            result = QMailAuthenticator::getResponse(svcCfg, challenge, responseAuthType, authPassword);
        }
#else
        result = QMailAuthenticator::getResponse(svcCfg, challenge);
#endif
    }

    return result;
}
#ifdef UOA_ENABLED
QString SmtpAuthenticator::getSaslCapabilities(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities)
{
    QStringList caps = capabilities;
    QString mech = svcCfg.value("uoaAuthMech"); // TODO: use proper auth mech values?
    if (mech != QStringLiteral("AUTOMATIC")) {
        if (capabilities.contains(mech)) {
            return mech;
        }
    }
    // Fall back to sasl picking most suitable method
    caps.removeDuplicates();
    return capabilities.join(QChar(' '));
}
#endif
