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
#include "PresetProviderModel.h"
#include <QSettings>
#include <Paths.h>

Provider::Provider(QObject *parent, const QString &description, const int &type):
    QObject(parent), m_description(description), m_type((Provider::Type)type)
{
}

Provider::Provider(QObject *parent, const QString &providerFilePath, const QString &group) : QObject(parent),
    m_imapPort(0), m_imapUseSSL(false), m_imapStartTLS(false), m_smtpPort(0), m_smtpUseSSL(false), m_smtpStartTLS(false),
    m_type(INVALID)
{
    ProviderFile file(providerFilePath, ProviderFile::NativeFormat);
    file.beginGroup(group);
    m_description = file.value(QStringLiteral("description")).toString();
    m_domain = file.value(QStringLiteral("domain")).toString();

    m_imapHost = file.value(QStringLiteral("imap.server")).toString();
    m_imapPort = file.value(QStringLiteral("imap.port")).toInt();
    m_imapUseSSL = file.value(QStringLiteral("imap.useSsl")).toBool();
    m_imapStartTLS = file.value(QStringLiteral("imap.startTls")).toBool();

    m_smtpHost = file.value(QStringLiteral("smtp.server")).toString();
    m_smtpPort = file.value(QStringLiteral("smtp.port")).toInt();
    m_smtpUseSSL = file.value(QStringLiteral("imap.useSsl")).toBool();
    m_smtpStartTLS = file.value(QStringLiteral("imap.startTls")).toBool();

    m_type = Type::PRESET;
    file.endGroup();
}

QString Provider::icon()
{
    switch (m_type) {
    case INVALID:
    case PRESET:
        return Paths::providerIconForDomain(m_domain);
    case IMAP:
    case POP3:
        return QStringLiteral("qrc:/provider/dekko-app-symbolic.svg");
    case SMTP:
        return QStringLiteral("qrc:/actions/send.svg");
    }
    Q_UNREACHABLE();
}

PresetProviderModel::PresetProviderModel(QObject *parent) : QObject(parent),
    m_model(0)
{
    m_model = new QQmlObjectListModel<Provider>(this);
    emit modelChanged();
    m_providers = Paths::findProviderFile();
    init();
}

void PresetProviderModel::init()
{
    ProviderFile p(m_providers, ProviderFile::NativeFormat);
#ifdef IMAP_ENABLED
    Q_FOREACH(const QString &group, p.childGroups()) {
        Provider *provider = new Provider(0, m_providers, group);
        m_model->append(provider);
    }
    Provider *imapprovider = new Provider(0, "IMAP", Provider::IMAP);
    m_model->append(imapprovider);
#endif
#ifdef POP3_ENABLED
    Provider *pop3provider = new Provider(0, "POP3", Provider::POP3);
    m_model->append(pop3provider);
#endif
    Provider *smtpprovider = new Provider(0, "SMTP", Provider::SMTP);
    m_model->append(smtpprovider);
}



