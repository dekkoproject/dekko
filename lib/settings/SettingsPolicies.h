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
#include "SettingsObjectBase.h"

/**
 * @brief The PolicyInterface class
 *
 * Interface that all policies should implement.
 *
 * A Policy is a group of related configurations/settings.
 */
class PolicyInterface
{
public:
    /**
     * @brief readPolicy
     * @param policy
     * @return policy value
     */
    virtual QString readPolicy(const QString &policy) = 0;
    /**
     * @brief setPolicy
     * @param policy
     * @param value
     */
    virtual void setPolicy(const QString &policy, const QString &value) = 0;
    /**
     * @brief setDefaults
     */
    virtual void setDefaults() = 0;
};
Q_DECLARE_INTERFACE(PolicyInterface, "PolicyInterface")

/**
 * @brief The GlobalPolicy class
 *
 * This is the base class for all application wide policies. Each derived policy
 * will get it's own atomic settings file located in the cache directory. You
 * need to set the policies key in the constructor \see setSettingsKey().
 *
 * You also need to re-implement setDefaults() and createDefaultsIfNotExist(),
 * usually createDefaultsIfNotExist() can just directly call setDefaults() but is
 * left to the subclass implementor to do incase they want to add some extra logic
 * before creating the default values.
 */
class GlobalPolicy : public SettingsObjectBase, public PolicyInterface
{
    Q_OBJECT
    Q_INTERFACES(PolicyInterface)
public:
    /**
     * @brief GlobalPolicy
     * @param parent
     */
    explicit GlobalPolicy(QObject *parent) : SettingsObjectBase(parent) {}

    // PolicyInterface interface
public:
    virtual QString readPolicy(const QString &policy) override {
        return read(policy, QString()).toString();
    }
    virtual void setPolicy(const QString &policy, const QString &value) override {
        write(policy, value);
    }
    virtual void setDefaults() override {}

    // SettingsObjectBase interface
protected:
    virtual void createDefaultsIfNotExist() override {}
};

class PrivacyPolicy : public GlobalPolicy
{
    Q_OBJECT
    /**
     * @brief Allow automatic loading of remote content
     * @accessors %allowRemoteContent(), setAllowRemoteContent()
     */
    Q_PROPERTY(bool allowRemoteContent READ allowRemoteContent WRITE setAllowRemoteContent NOTIFY policyChanged)
    /**
     * @brief Allow loading of images
     *
     * This differs slightly to allowRemoteContent as that blocks all network requests from a message body. i.e tracking links et al.
     *
     * This will only prevent images from loading in the message body.
     * Usually we would want this to always be true and leave allowRemoteContent to handle the privacy stuff.
     *
     * @accessors %autoLoadImages(), setAutoLoadImages()
     */
    Q_PROPERTY(bool autoLoadImages READ autoLoadImages WRITE setAutoLoadImages NOTIFY policyChanged)
public:
    explicit PrivacyPolicy(QObject *parent = 0);

    bool allowRemoteContent();
    void setAllowRemoteContent(const bool allowed);

    bool autoLoadImages();
    void setAutoLoadImages(const bool autoLoad);

    // PolicyInterface interface
public:
    virtual void setDefaults() override;

signals:
    void policyChanged();

    // SettingsObjectBase interface
protected:
    virtual void createDefaultsIfNotExist() override;
};

class ViewPolicy : public GlobalPolicy
{
    Q_OBJECT
    /**
     * @brief Crontrol and retain the state of the unified inbox expansion
     * @accessors %unifiedInboxExpanded(), setUnifiedInboxExpanded()
     */
    Q_PROPERTY(bool unifiedInboxExpanded READ unifiedInboxExpanded WRITE setUnifiedInboxExpanded NOTIFY policyChanged)
    /**
     * @brief Crontrol and retain the state of the favourite folders expansion
     * @accessors %favouritesExpanded(), favouritesExpanded()
     */
    Q_PROPERTY(bool favouritesExpanded READ favouritesExpanded WRITE setFavouritesExpanded NOTIFY policyChanged)
    Q_PROPERTY(bool favouritesVisible READ favouritesVisible WRITE setFavouritesVisible NOTIFY policyChanged)
    Q_PROPERTY(bool smartFoldersExpanded READ smartFoldersExpanded WRITE setSmartFoldersExpanded NOTIFY policyChanged)
    Q_PROPERTY(bool smartFoldersVisible READ smartFoldersVisible WRITE setSmartFoldersVisible NOTIFY policyChanged)
    Q_PROPERTY(bool accountsExpanded READ accountsExpanded WRITE setAccountsExpanded NOTIFY policyChanged)
    Q_PROPERTY(bool accountsVisible READ accountsVisible WRITE setAccountsVisible NOTIFY policyChanged)
    Q_PROPERTY(bool gravatarEnabled READ gravatarEnabled WRITE setGravatarEnabled NOTIFY policyChanged)
    Q_PROPERTY(bool hideMarkedDeleted READ hideMarkedDeleted WRITE setHideMarkedDeleted NOTIFY policyChanged)
    Q_PROPERTY(bool preferPlainText READ preferPlainText WRITE setPreferPlainText NOTIFY policyChanged)
    Q_PROPERTY(int previewLines READ previewLines WRITE setPreviewLines NOTIFY policyChanged)
    Q_PROPERTY(bool threadViewEnabled READ threadViewEnabled WRITE setThreadViewEnabled NOTIFY policyChanged)
    Q_PROPERTY(bool showToasts READ showToasts WRITE setShowToasts NOTIFY policyChanged)
public:
    explicit ViewPolicy(QObject *parent = 0);

    bool unifiedInboxExpanded();
    void setUnifiedInboxExpanded(const bool expanded);

    bool favouritesExpanded();
    void setFavouritesExpanded(const bool expanded);

    bool favouritesVisible();
    void setFavouritesVisible(const bool visible);

    bool smartFoldersExpanded();
    void setSmartFoldersExpanded(const bool expanded);

    bool smartFoldersVisible();
    void setSmartFoldersVisible(const bool visible);

    bool accountsExpanded();
    void setAccountsExpanded(const bool expanded);

    bool accountsVisible();
    void setAccountsVisible(const bool visible);

    bool gravatarEnabled();
    void setGravatarEnabled(const bool enabled);

    bool hideMarkedDeleted();
    void setHideMarkedDeleted(const bool hide);

    bool preferPlainText();
    void setPreferPlainText(const bool prefer);

    int previewLines();
    void setPreviewLines(const int &lines);

    bool threadViewEnabled();
    void setThreadViewEnabled(const bool enabled);

    bool showToasts();
    void setShowToasts(const bool show);
signals:
    void policyChanged();
    // PolicyInterface interface
public:
    virtual void setDefaults() override;

    // SettingsObjectBase interface
protected:
    virtual void createDefaultsIfNotExist() override {
        setDefaults();
    }
};

/**
 * @brief The AccountPolicy class
 */
class AccountPolicy : public QObject, public PolicyInterface
{
    Q_OBJECT
    Q_INTERFACES(PolicyInterface)
    /**
     * @brief The account id this policy belongs to
     * @accessors %accountId(), setAccountId()
     */
    Q_PROPERTY(int accountId READ accountId WRITE setAccountId NOTIFY accountIdChanged)

public:
    explicit AccountPolicy(QObject *parent = 0) : QObject(parent) {}
    AccountPolicy(QObject *parent, const QMailAccountId &id): QObject(parent), m_accountId(id) {}
    AccountPolicy(QObject *parent, const int &id): QObject(parent), m_accountId(QMailAccountId(id)) {}

    int accountId() const;
    void setAccountId(const int &id);
    void setAccountId(const QMailAccountId &id);
    // policy interfact
    Q_INVOKABLE virtual void setPolicy(const QString &policy, const QString &value) override;
    Q_INVOKABLE virtual QString readPolicy(const QString &policy) override;
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
class MailPolicy :  public AccountPolicy
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
    explicit MailPolicy(QObject *parent = 0);
    MailPolicy(QObject *parent, const int &id);
    MailPolicy(QObject *parent, const QMailAccountId &id);

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

#endif // SETTINGSPOLICIES_H
