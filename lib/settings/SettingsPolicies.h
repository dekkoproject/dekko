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
#ifndef SETTINGSPOLICIES_H
#define SETTINGSPOLICIES_H

#include <QObject>
#include <qmailaccount.h>
#include <qmailstore.h>

/**
 * @brief The BasePolicy class
 */
class BasePolicy : public QObject
{
    Q_OBJECT
    /**
     * @brief The account id this policy belongs to
     * @accessors %accountId(), setAccountId()
     */
    Q_PROPERTY(int accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)

public:
    explicit BasePolicy(QObject *parent = 0) : QObject(parent) {}
    BasePolicy(QObject *parent, const QMailAccountId &id): QObject(parent), m_accountId(id) {}
    BasePolicy(QObject *parent, const int &id): QObject(parent), m_accountId(QMailAccountId(id)) {}

    int accountId() const { return m_accountId.toULongLong(); }
    void setAccountId(const int &id) {
        setAccountId(QMailAccountId(id));
    }
    void setAccountId(const QMailAccountId &id) {
        m_accountId = id;
        emit accountIdChanged();
    }
    Q_INVOKABLE void setPolicy(const QString &policy, const QString &value) {
        QMailAccount account(m_accountId);
        account.setCustomField(QString("policy.%1").arg(policy), value);
        QMailStore::instance()->updateAccount(&account);
    }
    Q_INVOKABLE QString readPolicy(const QString &policy) {
        return QMailAccount(m_accountId).customField(QString("policy.%1").arg(policy));
    }
    virtual void setDefaults() {}

signals:
    void accountIdChanged();
    void accountUpdated();

protected:
    bool idValid() { return QMailAccountId(m_accountId).isValid(); }

private:
    QMailAccountId m_accountId;
};

/**
 * @brief The MailPolicy class
 */
class MailPolicy :  public BasePolicy
{
    Q_OBJECT
    /**
     * @brief How long to wait before marking a message read
     * @accessors markInterval(), setMarkInterval()
     */
    Q_PROPERTY(int markAsReadInterval READ markInterval WRITE setMarkInterval NOTIFY policyChanged)
    /**
     * @brief The MarkAsRead policy applied to just opened messages
     * @accessors %markRead(), setMarkRead()
     */
    Q_PROPERTY(MarkReadMode markRead READ markRead WRITE setMarkRead NOTIFY policyChanged)
    Q_ENUMS(MarkReadMode)

public:
    explicit MailPolicy(QObject *parent = 0) : BasePolicy(parent) {
        connect(this, &MailPolicy::accountIdChanged, this, &MailPolicy::policyChanged);
    }
    MailPolicy(QObject *parent, const int &id) : BasePolicy(parent, id) {
        connect(this, &MailPolicy::accountIdChanged, this, &MailPolicy::policyChanged);
    }
    MailPolicy(QObject *parent, const QMailAccountId &id) : BasePolicy(parent, id) {
        connect(this, &MailPolicy::accountIdChanged, this, &MailPolicy::policyChanged);
    }

    /**
     * @brief The MarkReadMode enum
     *
     * The mode applied to a recently opened message. This is evaluated
     * after the main body part has been displayed on the screen as there maybe
     * a delay between the message being fetched and it actually being displayed
     */
    enum MarkReadMode {
        Never, /**< @short Never automatically mark the message read */
        AfterInterval,/**< @short Mark message read after set interval \see markAsReadInterval() */
        Immediately /**< @short Mark message read immediately */
    };

    /** @short How long to wait before marking a message as read. */
    int markInterval();
    /** @short Set interval for how long to wait before marking a message as read. */
    void setMarkInterval(const int &interval);
    /** @short The MarkAsRead policy applied to just opened messages. */
    MarkReadMode markRead();
    /** @short Set the MarkAsRead policy applied to just opened messages */
    void setMarkRead(const MarkReadMode mode);

    virtual void setDefaults() override;

signals:
    void policyChanged();

};

///**
// * @brief The PrivacyPolicy class
// */
//class PrivacyPolicy : public BasePolicy
//{
//    Q_OBJECT
//public:
//};

//class ViewPolicy : public BasePolicy
//{
//    Q_OBJECT
//};

//class ContactsPolicy : public BasePolicy
//{
//    Q_OBJECT
//};

#endif // SETTINGSPOLICIES_H
