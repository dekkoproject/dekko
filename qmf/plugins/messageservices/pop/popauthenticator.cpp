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

#include "popauthenticator.h"
#include "popconfiguration.h"
#include <qmailauthenticator.h>
#include <qmailtransport.h>
#include <qmaillog.h>

#ifdef USE_ACCOUNTS_QT
namespace {

QString authPassword;
QMail::SaslMechanism responseAuthType;

}
#endif

bool PopAuthenticator::useEncryption(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities)
{
#ifdef QT_NO_OPENSSL
    Q_UNUSED(svcCfg)
    Q_UNUSED(capabilities)
    return false;
#else
    PopConfiguration popCfg(svcCfg);
    bool useTLS(popCfg.mailEncryption() == QMailTransport::Encrypt_TLS);

    if (!capabilities.contains("STLS")) {
        if (useTLS) {
            qWarning() << "Server does not support TLS - continuing unencrypted";
        }
    } else {
        if (useTLS) {
            return true;
        } else {
            if (!capabilities.contains("USER")) {
                qWarning() << "Server does not support unencrypted USER - using encryption";
                return true;
            }
        }
    }

    return QMailAuthenticator::useEncryption(svcCfg, capabilities);
#endif
}

#ifdef USE_ACCOUNTS_QT
QList<QByteArray> PopAuthenticator::getAuthentication(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities, const QMap<QString, QList<QByteArray> > &ssoLogin)
{
    Q_UNUSED(capabilities);
    QList<QByteArray> result;

    // Currently pop account does not have any auth settings, so only plain can be used
    if(ssoLogin.isEmpty()){
        PopConfiguration popCfg(svcCfg);
        qMailLog(POP) << Q_FUNC_INFO << "SSO identity is not found for account id: "<< popCfg.id()
                      << ", returning empty authentication";
    } else {
        result = ssoLogin.value("PLAIN");
    }
    return result;
}
#else
QList<QByteArray> PopAuthenticator::getAuthentication(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QStringList &capabilities)
{
    QList<QByteArray> result;

    QByteArray auth(QMailAuthenticator::getAuthentication(svcCfg, capabilities));
    if (!auth.isEmpty()) {
        result.append(QByteArray("AUTH ") + auth);
    } else {
        // If not handled by the authenticator, fall back to user/pass
        PopConfiguration popCfg(svcCfg);

        result.append(QByteArray("USER ") + popCfg.mailUserName().toLatin1());
        result.append(QByteArray("PASS ") + popCfg.mailPassword().toLatin1());
    }

    return result;
}
#endif
QByteArray PopAuthenticator::getResponse(const QMailAccountConfiguration::ServiceConfiguration &svcCfg, const QByteArray &challenge)
{
#ifdef USE_ACCOUNTS_QT
    return QMailAuthenticator::getResponse(svcCfg, challenge, responseAuthType, authPassword);
#else
    return QMailAuthenticator::getResponse(svcCfg, challenge);
#endif
}

