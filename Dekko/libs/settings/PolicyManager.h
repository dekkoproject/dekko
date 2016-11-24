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
#ifndef POLICYMANAGER_H
#define POLICYMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QPointer>
#include "SettingsPolicies.h"
/**
 * @brief The PolicyManager class
 */
class PolicyManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PrivacyPolicy *privacy READ privacyPolicy CONSTANT)
    Q_PROPERTY(ViewPolicy *views READ viewPolicy CONSTANT)
public:
    explicit PolicyManager(QObject *parent = 0);

    /**
     * @brief Singleton accessor for the PolicyManager
     * @return PolicyManager instance
     */
    static PolicyManager *instance();
    /**
     * @brief factory for accessiong singleton in QML
     * @param engine
     * @param scriptEngine
     * @return PolicyManager instance
     */
    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

    /**
     * @brief Retrieve the MailPolicy for \param accountId
     * @param QMailAccountId::toULongLong()
     * @return MailPolicy
     */
    Q_INVOKABLE MailPolicy *mailPolicy(const int &accountId);
    /**
     * @brief Retrieve the MailPolicy for \param accountId
     * @param QMailAccountId
     * @return MailPolicy
     */
    MailPolicy *mailPolicy(const QMailAccountId &accountId);

    /**
     * @brief Retrieve the PrivacyPolicy for this user
     * @return PrivacyPolicy
     */
    PrivacyPolicy *privacyPolicy();

    /**
     * @brief Retrieve the ViewPolicy for this user
     * @return ViewPolicy
     */
    ViewPolicy *viewPolicy();

    /**
     * @brief Set the default policies for \param accountId
     * @param QMailAccountId::toULongLong()
     */
    Q_INVOKABLE void setDefaultPolicies(const int &accountId);
    /**
     * @brief Set the default policies for \param accountId
     * @param QMailAccountId
     */
    void setDefaultPolicies(const QMailAccountId &accountId);

    Q_INVOKABLE bool idValid(const int &id);

private:
    QPointer<PrivacyPolicy> m_privacy;
    QPointer<ViewPolicy> m_view;
};

#endif // POLICYMANAGER_H
