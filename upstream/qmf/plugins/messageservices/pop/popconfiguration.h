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

#ifndef POPCONFIGURATION_H
#define POPCONFIGURATION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt Extended API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmailserviceconfiguration.h>

class PLUGIN_EXPORT PopConfiguration : public QMailServiceConfiguration
{
public:
    explicit PopConfiguration(const QMailAccountConfiguration &config);
    explicit PopConfiguration(const QMailAccountConfiguration::ServiceConfiguration &svcCfg);

    QString mailUserName() const;
    QString mailPassword() const;
    QString mailServer() const;
    int mailPort() const;
    int mailEncryption() const;

    bool canDeleteMail() const;
    bool deleteRetrievedMailsFromServer() const;

    bool isAutoDownload() const;
    int maxMailSize() const;

    int checkInterval() const;
    bool intervalCheckRoamingEnabled() const;

#ifndef QT_NO_OPENSSL
    bool acceptUntrustedCertificates() const;
#endif
};

class PLUGIN_EXPORT PopConfigurationEditor : public PopConfiguration
{
public:
    explicit PopConfigurationEditor(QMailAccountConfiguration *config);

    void setMailUserName(const QString &str);
    void setMailPassword(const QString &str);
    void setMailServer(const QString &str);
    void setMailPort(int i);
#ifndef QT_NO_OPENSSL
    void setMailEncryption(int t);
    void setAcceptUntrustedCertificates(bool v);
#endif

    void setDeleteMail(bool b);

    void setAutoDownload(bool autodl);
    void setMaxMailSize(int i);

    void setCheckInterval(int i);
    void setIntervalCheckRoamingEnabled(bool enabled);
};

#endif
