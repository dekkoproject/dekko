#ifndef NAVIGATIONSETTINGS_H
#define NAVIGATIONSETTINGS_H

#include <QObject>
#include <QVariantMap>
#include <QLockFile>
#include <QScopedPointer>
#include "SettingsObjectBase.h"

struct NavigationKeys {
    static const QString defaultsCreated, unifiedInboxExpanded;
    static const QString favourites, favouriteExpanded, showFavourites;
    static const QString smarFolders, showSmartFolders, smartFoldersExpanded;
    static const QString accounts, showAccounts, accountsExpanded;
};

class NavigationSettings : public SettingsObjectBase
{
    Q_OBJECT
    Q_ENUMS(Keys)
public:
    explicit NavigationSettings(QObject *parent = 0);

    enum Keys {
        DefaultsCreated = 0,
        UnifiedInboxExpanded,
        Favourites,
        FavouritesExpanded,
        FavouritesVisible,
        SmartFolders,
        SmartFoldersVisible,
        SmartFoldersExpanded,
        Accounts,
        AccountsVisible,
        AccountsExpanded
    };

    QString keyValue(Keys key);
    Q_INVOKABLE QVariant get(Keys key);
    Q_INVOKABLE void set(Keys key, const QJsonValue &value);

private:
    void createDefaultsIfNotExist();

    QScopedPointer<QLockFile> *m_lock;

};

#endif // NAVIGATIONSETTINGS_H
