/**************************************************************************
   Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
**************************************************************************/
#include "PolicyManager.h"
#include <QPointer>
#include <QScopedPointer>

PolicyManager::PolicyManager(QObject *parent) : QObject(parent)
{
    // Maintain instances of global policies
    m_privacy = new PrivacyPolicy(this);
    m_view = new ViewPolicy(this);
}

static QPointer<PolicyManager> s_mgr;
PolicyManager *PolicyManager::instance()
{
    if (s_mgr.isNull()) {
        s_mgr = new PolicyManager();
    }
    return s_mgr;
}

QObject *PolicyManager::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return PolicyManager::instance();
}

MailPolicy *PolicyManager::mailPolicy(const int &accountId)
{
    return mailPolicy(QMailAccountId(accountId));
}

MailPolicy *PolicyManager::mailPolicy(const QMailAccountId &accountId)
{
    return new MailPolicy(this, accountId);
}

PrivacyPolicy *PolicyManager::privacyPolicy()
{
    if (m_privacy->isValid()) {
        return m_privacy;
    }
    return Q_NULLPTR;
}

ViewPolicy *PolicyManager::viewPolicy()
{
    return m_view;
}

void PolicyManager::setDefaultPolicies(const int &accountId) {
    setDefaultPolicies(QMailAccountId(accountId));
}

void PolicyManager::setDefaultPolicies(const QMailAccountId &accountId)
{
    QMailAccountId id(accountId);
    if (id.isValid()) {
        mailPolicy(accountId)->setDefaults();
    }
}

bool PolicyManager::idValid(const int &id)
{
    return QMailAccountId(id).isValid();
}

