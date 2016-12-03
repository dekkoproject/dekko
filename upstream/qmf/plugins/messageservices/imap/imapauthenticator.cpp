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

#include "imapauthenticator.h"

#include "imapprotocol.h"
#include "imapconfiguration.h"

#include <qmailauthenticator.h>
#include <qmailtransport.h>
#include <qmailnamespace.h>
#include <qmaillog.h>
#include <qmailstore.h>

namespace {


#ifdef USE_ACCOUNTS_QT
QString authPassword;
QMail::SaslMechanism responseAuthType;
QByteArray gResponse;
#endif
}

bool ImapAuthenticator::useEncryption(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities)
{
#ifdef QT_NO_OPENSSL
    Q_UNUSED(svcCfg)
    Q_UNUSED(capabilities)
    return false;
#else
    ImapConfiguration imapCfg(svcCfg);
    bool useTLS(imapCfg.mailEncryption() == QMailTransport::Encrypt_TLS);

    if (!capabilities.contains("STARTTLS")) {
        if (useTLS) {
            qWarning() << "Server does not support TLS - continuing unencrypted";
        }
    } else {
        if (useTLS) {
            return true;
        }
    }

    return QMailAuthenticator::useEncryption(svcCfg, capabilities);
#endif
}
#ifdef USE_ACCOUNTS_QT
// Returns authentication type from more secure to less secure supported
static QMail::SaslMechanism authFromCapabilities(const QStringList &capabilities, const QMap<QString, QList<QByteArray> > &ssoLogin)
{
    QStringList authCaps;
    foreach (QString const& capability, capabilities) {
        if (capability.startsWith("AUTH=", Qt::CaseInsensitive)) {
            authCaps.append(capability.mid(5));
        }
    }

    qMailLog(IMAP) << "Auths found: " << authCaps;

    if (authCaps.contains("CRAM-MD5", Qt::CaseInsensitive) && ssoLogin.contains("CRAM-MD5")) {
        qMailLog(IMAP) << "Returning auth CRAM-MD5";
        return QMail::CramMd5Mechanism;
    } else if (!authCaps.contains("PLAIN", Qt::CaseInsensitive)
               && !capabilities.contains("LOGINDISABLED", Qt::CaseInsensitive) && ssoLogin.contains("LOGIN")) {
        qMailLog(IMAP) <<  "Returning auth LOGIN";
        // According to RFC3501, LOGIN should be used as last resort(for retro-compatibility)
        // We should check that plain is not advertised(this can be omitted even if server supports it),
        // and that LOGINDISABLED capability is not advertised.
        return QMail::LoginMechanism;
    } else if (ssoLogin.contains("PLAIN")) {
        qMailLog(IMAP) << "Returning auth PLAIN";
        // According to RFC3501, IMAP4 servers MUST implement plain auth
        return QMail::PlainMechanism;
    } else {
        // return empty auth in order to send empty reply and make auth process fail
        qMailLog(IMAP) << "Returning auth NONE";
        return QMail::NoMechanism;
    }
}

static QByteArray authenticationResponses(QList<QByteArray> &authList, const QMail::SaslMechanism &authType, const QMailAccountId &id)
{
    QByteArray result;
    if(!authList.empty()) {
        result = authList.takeFirst();
        if (!authList.empty()) {
            if (authType == QMail::CramMd5Mechanism) {
                authPassword = QString::fromLatin1(authList.takeFirst());
                responseAuthType = QMail::CramMd5Mechanism;
            } else {
                gResponse = authList.first();
                // We need to clean this state, both the other auths available will work the same way
                responseAuthType = QMail::PlainMechanism;
            }
        }
    } else {
        qMailLog(IMAP) << "Failed to get authentication for method" << authType << "in account id:" << id;
    }
    return result;
}


QByteArray ImapAuthenticator::getAuthentication(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities, const QMap<QString, QList<QByteArray> > &ssoLogin)
{
    QByteArray result;
    ImapConfiguration imapCfg(svcCfg);
    QMailAccountId id(imapCfg.id());
    QMail::SaslMechanism authType = static_cast<QMail::SaslMechanism>(imapCfg.mailAuthentication());

    // if we don't have auth yet, try to get it from the capabilities
    if (ssoLogin.size() > 1 && authType == QMail::NoMechanism) {
        qMailLog(IMAP) << "Discovering authentication from capabilities for account id:" << id;
        authType = authFromCapabilities(capabilities, ssoLogin);
        if (authType != QMail::NoMechanism) {
            QMailAccount account(id);
            QMailAccountConfiguration accountConfig(id);
            QMailAccountConfiguration::ServiceConfiguration serviceConf(accountConfig.serviceConfiguration("imap4"));
            serviceConf.setValue("authentication",QString::number(authType));
            if (!QMailStore::instance()->updateAccount(&account, &accountConfig)) {
                qWarning() << "Unable to update account" << account.id() << "to auth type!!!!";
            }
        }
    }

    if (authType != QMail::NoMechanism) {
        if (!ssoLogin.isEmpty()) {
            QList<QByteArray> auth;

            if (ssoLogin.size() == 1) {
                QList<QString> keys = ssoLogin.keys();
                auth = ssoLogin.value(keys.at(0));
                result = authenticationResponses(auth, authType, id);
                qMailLog(IMAP) << "Using authentication method " << keys.at(0)
                               << " for account id:" << id;
            } else {
                if (authType == QMail::CramMd5Mechanism) {
                    auth = ssoLogin.value("CRAM-MD5");
                    result = authenticationResponses(auth, authType, id);
                } else if (authType == QMail::PlainMechanism) {
                    auth = ssoLogin.value("PLAIN");
                    result = authenticationResponses(auth, authType, id);
                } else if (authType == QMail::LoginMechanism) {
                    auth = ssoLogin.value("LOGIN");
                    result = authenticationResponses(auth, authType, id);
                }
            }
        } else {
            qMailLog(IMAP) << Q_FUNC_INFO << "SSO identity is not found for account id: "<< id
                           << ", returning empty authentication";
        }
    }
    return result;
}
#else
QByteArray ImapAuthenticator::getAuthentication(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities)
{
    QByteArray result(QMailAuthenticator::getAuthentication(svcCfg, capabilities));
    if (!result.isEmpty())
        return QByteArray("AUTHENTICATE ") + result;

    // If not handled by the authenticator, fall back to login
    ImapConfiguration imapCfg(svcCfg);
    if (imapCfg.mailAuthentication() == QMail::PlainMechanism) {
        QByteArray username(imapCfg.mailUserName().toLatin1());
        QByteArray password(imapCfg.mailPassword().toLatin1());
        return QByteArray("AUTHENTICATE PLAIN ") + QByteArray(username + '\0' + username + '\0' + password).toBase64();
    }

    return QByteArray("LOGIN") + ' ' + ImapProtocol::quoteString(imapCfg.mailUserName().toLatin1())
                               + ' ' + ImapProtocol::quoteString(imapCfg.mailPassword().toLatin1());
}
#endif
QByteArray ImapAuthenticator::getResponse(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QByteArray &challenge)
{
#ifdef USE_ACCOUNTS_QT
    if (responseAuthType == QMail::CramMd5Mechanism) {
        return QMailAuthenticator::getResponse(svcCfg, challenge, responseAuthType, authPassword);
    }
    return gResponse;
#else
    return QMailAuthenticator::getResponse(svcCfg, challenge);
#endif
}
#ifdef UOA_ENABLED
QString ImapAuthenticator::getSaslCapabilities(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities)
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



