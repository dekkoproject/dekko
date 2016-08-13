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
#include "SettingsPolicies.h"

int AccountPolicy::accountId() const { return m_accountId.toULongLong(); }

void AccountPolicy::setAccountId(const int &id) {
    setAccountId(QMailAccountId(id));
}

void AccountPolicy::setAccountId(const QMailAccountId &id) {
    m_accountId = id;
    emit accountIdChanged();
}

void AccountPolicy::setPolicy(const QString &policy, const QString &value) {
    QMailAccount account(m_accountId);
    account.setCustomField(QString("policy.%1").arg(policy), value);
    QMailStore::instance()->updateAccount(&account);
}

QString AccountPolicy::readPolicy(const QString &policy) {
    return QMailAccount(m_accountId).customField(QString("policy.%1").arg(policy));
}

MailPolicy::MailPolicy(QObject *parent) : AccountPolicy(parent) {
    connect(this, &MailPolicy::accountIdChanged, this, &MailPolicy::policyChanged);
}

MailPolicy::MailPolicy(QObject *parent, const int &id) : AccountPolicy(parent, id) {
    connect(this, &MailPolicy::accountIdChanged, this, &MailPolicy::policyChanged);
}

MailPolicy::MailPolicy(QObject *parent, const QMailAccountId &id) : AccountPolicy(parent, id) {
    connect(this, &MailPolicy::accountIdChanged, this, &MailPolicy::policyChanged);
}

int MailPolicy::markInterval()
{
    if (idValid()) {
        return readPolicy(QStringLiteral("mail.markReadInterval")).toInt();
    }
    return 1000;
}

void MailPolicy::setMarkInterval(const int &interval)
{
    if (!idValid()) {
        qDebug() << "Invalid ID cannot set interval policy";
    }
    setPolicy(QStringLiteral("mail.markReadInterval"), QString::number(interval));
    emit policyChanged();
}

MailPolicy::MarkReadMode MailPolicy::markRead()
{
    QString policy = readPolicy(QStringLiteral("mail.markAsRead"));
    if (!policy.isEmpty()) {
        if (policy == QStringLiteral("never")) {
            return MarkReadMode::Never;
        } else if ( policy == QStringLiteral("interval")) {
            return MarkReadMode::AfterInterval;
        } else if (policy == QStringLiteral("immediate")) {
            return MarkReadMode::Immediately;
        }
    }
    return MarkReadMode::AfterInterval;
}

void MailPolicy::setMarkRead(const MailPolicy::MarkReadMode mode)
{
    switch(mode) {
    case MarkReadMode::Never:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("never"));
        break;
    case MarkReadMode::AfterInterval:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("interval"));
        break;
    case MarkReadMode::Immediately:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("immediate"));
        break;
    default:
        setPolicy(QStringLiteral("mail.markAsRead"), QStringLiteral("interval"));
        break;
    }
    emit policyChanged();
}

void MailPolicy::setDefaults()
{
    setMarkRead(MarkReadMode::AfterInterval);
    setMarkInterval(1000);
}



PrivacyPolicy::PrivacyPolicy(QObject *parent) : GlobalPolicy(parent) {
    setSettingsKey(QStringLiteral("privacy"));
    connect(this, &PrivacyPolicy::dataChanged, this, &PrivacyPolicy::policyChanged);
}

bool PrivacyPolicy::allowRemoteContent() {
    return readPolicy(QStringLiteral("remote.contentAllowed")).toInt() != 0;
}

void PrivacyPolicy::setAllowRemoteContent(const bool allowed) {
    setPolicy(QStringLiteral("remote.contentAllowed"), QString::number(allowed ? 1 : 0));
}

bool PrivacyPolicy::autoLoadImages() {
    return readPolicy(QStringLiteral("remote.autoLoadImages")).toInt() != 0;
}

void PrivacyPolicy::setAutoLoadImages(const bool autoLoad) {
    setPolicy(QStringLiteral("remote.autoLoadImages"), QString::number(autoLoad ? 1 : 0));
}

void PrivacyPolicy::setDefaults() {
    if (read("defaults_created").toBool()) {
        return;
    }
    setAllowRemoteContent(false);
    // auto load images by default.
    // allow remote content will block these until they are allowed anyway
    setAutoLoadImages(true);
    write("defaults_created", true);
}

void PrivacyPolicy::createDefaultsIfNotExist() {
    setDefaults();
}

ViewPolicy::ViewPolicy(QObject *parent) : GlobalPolicy(parent) {
    connect(this, &ViewPolicy::dataChanged, this, &ViewPolicy::policyChanged);
    setSettingsKey(QStringLiteral("views"));
}

bool ViewPolicy::unifiedInboxExpanded()
{
    return readPolicy(QStringLiteral("navigation.unified.expanded")).toInt() != 0;
}

void ViewPolicy::setUnifiedInboxExpanded(const bool expanded)
{
    setPolicy(QStringLiteral("navigation.unified.expanded"), QString::number(expanded ? 1 : 0));
}

bool ViewPolicy::favouritesExpanded()
{
    return readPolicy(QStringLiteral("navigation.favourites.expanded")).toInt() != 0;
}

void ViewPolicy::setFavouritesExpanded(const bool expanded)
{
    setPolicy(QStringLiteral("navigation.favourites.expanded"), QString::number(expanded ? 1 : 0));
}

bool ViewPolicy::favouritesVisible()
{
    return readPolicy(QStringLiteral("navigation.favourites.visible")).toInt() != 0;
}

void ViewPolicy::setFavouritesVisible(const bool visible)
{
    setPolicy(QStringLiteral("navigation.favourites.visible"), QString::number(visible ? 1 : 0));
}

bool ViewPolicy::smartFoldersExpanded()
{
    return readPolicy(QStringLiteral("navigation.smartfolders.expanded")).toInt() != 0;
}

void ViewPolicy::setSmartFoldersExpanded(const bool expanded)
{
    setPolicy(QStringLiteral("navigation.smartfolders.expanded"), QString::number(expanded ? 1 : 0));
}

bool ViewPolicy::smartFoldersVisible()
{
    return readPolicy(QStringLiteral("navigation.smartfolders.visible")).toInt() != 0;
}

void ViewPolicy::setSmartFoldersVisible(const bool visible)
{
    setPolicy(QStringLiteral("navigation.smartfolders.visible"), QString::number(visible ? 1 : 0));
}

bool ViewPolicy::accountsExpanded()
{
    return readPolicy(QStringLiteral("navigation.accounts.expanded")).toInt() != 0;
}

void ViewPolicy::setAccountsExpanded(const bool expanded)
{
    setPolicy(QStringLiteral("navigation.accounts.expanded"), QString::number(expanded ? 1 : 0));
}

bool ViewPolicy::accountsVisible()
{
    return readPolicy(QStringLiteral("navigation.accounts.visible")).toInt() != 0;
}

void ViewPolicy::setAccountsVisible(const bool visible)
{
    setPolicy(QStringLiteral("navigation.accounts.visible"), QString::number(visible ? 1 : 0));
}

bool ViewPolicy::gravatarEnabled()
{
    return readPolicy(QStringLiteral("avatars.gravatarEnabled")).toInt() != 0;
}

void ViewPolicy::setGravatarEnabled(const bool enabled)
{
    setPolicy(QStringLiteral("avatars.gravatarEnabled"), QString::number(enabled ? 1 : 0));
}

bool ViewPolicy::hideMarkedDeleted()
{
    return readPolicy(QStringLiteral("view.hideMarkedDeleted")).toInt() != 0;
}

void ViewPolicy::setHideMarkedDeleted(const bool hide)
{
    setPolicy(QStringLiteral("view.hideMarkedDeleted"), QString::number(hide ? 1 : 0));
}

bool ViewPolicy::preferPlainText()
{
    return readPolicy(QStringLiteral("view.preferPlainText")).toInt() != 0;
}

void ViewPolicy::setPreferPlainText(const bool prefer)
{
    setPolicy(QStringLiteral("view.preferPlainText"), QString::number(prefer ? 1 : 0));
}

int ViewPolicy::previewLines()
{
    return readPolicy(QStringLiteral("view.previewLines")).toInt();
}

void ViewPolicy::setPreviewLines(const int &lines)
{
    static int maxCount = 3;
    static int minCount = 1;

    int num = minCount;
    if (lines > maxCount) {
        num = maxCount;
    } else if (lines >= minCount) {
        num = lines;
    }
    setPolicy(QStringLiteral("view.previewLines"), QString::number(num));
}

bool ViewPolicy::threadViewEnabled()
{
    return readPolicy(QStringLiteral("view.threadViewEnabled")).toInt() != 0;
}

void ViewPolicy::setThreadViewEnabled(const bool enabled)
{
    setPolicy(QStringLiteral("view.threadViewEnabled"), QString::number(enabled ? 1 : 0));
}

bool ViewPolicy::showToasts()
{
    return readPolicy(QStringLiteral("view.showToasts")).toInt() != 0;
}

void ViewPolicy::setShowToasts(const bool show)
{
    setPolicy(QStringLiteral("view.showToasts"), QString::number(show ? 1 : 0));
}

void ViewPolicy::setDefaults() {
    if (read("defaults_created").toBool()) {
        return;
    }
    setUnifiedInboxExpanded(false);
    setFavouritesExpanded(false);
    setFavouritesVisible(false);
    setSmartFoldersExpanded(true);
    setSmartFoldersVisible(true);
    setAccountsExpanded(true);
    setAccountsVisible(true);
    setGravatarEnabled(true);
    setHideMarkedDeleted(true);
    setPreferPlainText(false);
    setThreadViewEnabled(false);
    setShowToasts(true);
    setPreviewLines(2);
    write("defaults_created", true);
}
