/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: Valério Valério <valerio.valerio@jollamobile.com>
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

#include "passwordplugin.h"
#include "qmailnamespace.h"

// From Qt Messaging Framework IMAP plugin
// Ensure a string is quoted, if required for IMAP transmission
// As per IMAP4 rfc 2060 section 4.2
QString quoteIMAPString(const QString& input)
{
    // We can't easily catch controls other than those caught by \\s...
    QRegExp atomSpecials("[\\(\\)\\{\\s\\*%\\\\\"\\]]");

    // The empty string must be quoted
    if (input.isEmpty())
        return QString("\"\"");

    if (atomSpecials.indexIn(input) == -1)
        return input;

    // We need to quote this string because it is not an atom
    QString result(input);

    QString::iterator begin = result.begin(), it = begin;
    while (it != result.end()) {
        // We need to escape any characters specially treated in quotes
        if ((*it) == '\\' || (*it) == '"') {
            int pos = (it - begin);
            result.insert(pos, '\\');
            it = result.begin() + (pos + 1);
        }
        ++it;
    }

    return QMail::quoteString(result);
}

QByteArray quoteIMAPString(const QByteArray& input)
{
    return quoteIMAPString(QString::fromLatin1(input)).toLatin1();
}

SSOPasswordPlugin::SSOPasswordPlugin(QObject *parent)
    : SSOAuthService(parent)
{
}

SSOPasswordPlugin::~SSOPasswordPlugin()
{
}

QMap<QString, QList<QByteArray> > SSOPasswordPlugin::getIMAPAuthentication(const QString &password,
                                                    const QString &username) const
{
    QMap<QString, QList<QByteArray> > result;

    // Add PLAIN auth
    QByteArray user(username.toUtf8());
    QByteArray pass(password.toUtf8());

    QList<QByteArray> plainAuth;
    plainAuth.append(QByteArray("AUTHENTICATE PLAIN"));
    plainAuth.append(QByteArray(user + '\0' + user + '\0' + pass));
    result.insert(QString::fromLatin1("PLAIN"), plainAuth);

    // Add LOGIN auth
    result.insert(QString::fromLatin1("LOGIN"), QList<QByteArray>() << QByteArray("LOGIN") + ' ' + quoteIMAPString(username.toUtf8())
                  + ' ' + quoteIMAPString(password.toUtf8()));

    // Add CRAM_MD5
    QList<QByteArray> cramAuth;
    cramAuth.append(QByteArray("AUTHENTICATE CRAM-MD5"));
    cramAuth.append(QByteArray(password.toUtf8()));
    result.insert(QString::fromLatin1("CRAM-MD5"), cramAuth);

    return result;
}

QMap<QString, QList<QByteArray> > SSOPasswordPlugin::getPOPAuthentication(const QString &password,
                                                   const QString &username) const
{
    QMap<QString, QList<QByteArray> > result;

    // Add PLAIN auth
    QList<QByteArray> plainAuth;
    plainAuth.append(QByteArray("USER ") + username.toUtf8());
    plainAuth.append(QByteArray("PASS ") + password.toUtf8());
    result.insert(QString::fromLatin1("PLAIN"), plainAuth);

    // Currently pop account does not have any auth settings, so only plain can be used

    // Add CRAM-MD5
    /*QList<QByteArray> cramAuth;
    cramAuth.append(QByteArray("AUTH CRAM-MD5"));
    cramAuth.append(QByteArray(password.toLatin1()));
    result.insert(QString::fromLatin1("CRAM-MD5"), cramAuth);*/

    return result;
}

QMap<QString, QList<QByteArray> > SSOPasswordPlugin::getSMTPAuthentication(const QString &password,
                                                    const QString &username) const
{
    QMap<QString, QList<QByteArray> > result;

    QByteArray user(username.toUtf8());
    QByteArray pass(password.toUtf8());

    // Add PLAIN auth
    QList<QByteArray> plainAuth;
    plainAuth.append(QByteArray("AUTH PLAIN ") + QByteArray(user + '\0' + user + '\0' + pass).toBase64());
    plainAuth.append(QByteArray(user + '\0' + user + '\0' + pass));
    result.insert(QString::fromLatin1("PLAIN"), plainAuth);

    // Add LOGIN auth
    QList<QByteArray> loginAuth;
    loginAuth.append(QByteArray("AUTH LOGIN"));
    loginAuth.append(QByteArray(user));
    loginAuth.append(QByteArray(pass));
    result.insert(QString::fromLatin1("LOGIN"), loginAuth);

    // Add CRAM-MD5 auth
    QList<QByteArray> cramAuth;
    cramAuth.append(QByteArray("AUTH CRAM-MD5"));
    cramAuth.append(QByteArray(pass));
    result.insert(QString::fromLatin1("CRAM-MD5"), cramAuth);

    return result;
}

SSOAuthService* SSOPasswordPlugin::createService()
{
    return this;
}

QString SSOPasswordPlugin::key() const
{
    return "password";
}

QMap<QString, QList<QByteArray> > SSOPasswordPlugin::authentication(const SignOn::SessionData &sessionData,
                                                const QString &serviceType, const QString &userName) const
{
    QString password = sessionData.Secret();
    QString username = sessionData.UserName();

    if (username.isEmpty())
        username = userName;

    if (serviceType == "imap4") {
        return getIMAPAuthentication(password, username);
    } else if (serviceType == "pop3") {
        return getPOPAuthentication(password, username);
    } else if (serviceType == "smtp") {
        return getSMTPAuthentication(password, username);
    } else {
        return QMap<QString, QList<QByteArray> >();
    }
}

void SSOPasswordPlugin::credentialsNeedUpdate(int accountId)
{
    // For the password method we don't do anything, messageserver
    // already informs the clients about login failed.
    Q_UNUSED(accountId);
}

SignOn::SessionData SSOPasswordPlugin::sessionData(const QString &accountProvider,  QVariantMap authParameters) const
{
    Q_UNUSED(accountProvider);
    Q_UNUSED(authParameters);

    SignOn::SessionData data;
    data.setUiPolicy(SignOn::NoUserInteractionPolicy);
    return data;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(password,SSOPasswordPlugin)
#endif
