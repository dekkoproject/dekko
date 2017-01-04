#include "SnapStandardPaths.h"
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QStandardPaths>
#include <QCoreApplication>

#define SNAP "SNAP"
#define SNAP_DATA "SNAP_DATA"
#define SNAP_COMMON "SNAP_COMMON"
#define SNAP_USER_DATA "SNAP_USER_DATA"
#define SNAP_USER_COMMON "SNAP_USER_COMMON"


static bool ssp_testMode = false;

static QString getPath(const QByteArray &type)
{
    return QString::fromLatin1(qgetenv(type));
}

static bool hasPath(const QByteArray &type)
{
    return !getPath(type).isEmpty();
}


QString SnapStandardPaths::writableLocation(SnapStandardPaths::StandardLocation location)
{
    const QString appName = QStringLiteral("/%1").arg(QCoreApplication::applicationName());
    const QString dataLoc = QStringLiteral("/.local/share");
    const QString cacheLoc = QStringLiteral("/.cache");
    const QString configLoc = QStringLiteral("/.config");
    switch (location) {
    case HomeLocation:
        if (hasPath(SNAP_USER_DATA))
            return getPath(SNAP_USER_DATA);
        else
            return QDir::homePath();
    case CommonHomeLocation:
        if (hasPath(SNAP_USER_COMMON))
            return getPath(SNAP_USER_COMMON);
        else
            return QDir::homePath();
    case CacheLocation:
        if (hasPath(SNAP_USER_DATA))
            return getPath(SNAP_USER_DATA) + cacheLoc;
        else
            return QDir::homePath() + cacheLoc;
    case CommonCacheLocation:
        if (hasPath(SNAP_USER_COMMON))
            return getPath(SNAP_USER_COMMON) + cacheLoc;
        else
            return QDir::homePath() + cacheLoc;
    case AppCacheLocation:
        return SnapStandardPaths::writableLocation(CacheLocation) + appName;
    case CommonAppCacheLocation:
        return SnapStandardPaths::writableLocation(CommonCacheLocation) + appName;
    case DataLocation:
        if (hasPath(SNAP_USER_DATA))
            return getPath(SNAP_USER_DATA) + dataLoc;
        else
            return QDir::homePath() + dataLoc;
    case CommonDataLocation:
        if (hasPath(SNAP_USER_COMMON))
            return getPath(SNAP_USER_COMMON) + dataLoc;
        else
            return QDir::homePath() + dataLoc;
    case AppDataLocation:
        return SnapStandardPaths::writableLocation(DataLocation) + appName;
    case CommonAppDataLocation:
        return SnapStandardPaths::writableLocation(CommonDataLocation) + appName;
    case ConfigLocation:
        if (hasPath(SNAP_USER_DATA))
            return getPath(SNAP_USER_DATA) + configLoc;
        else
            return QDir::homePath() + configLoc;
    case CommonConfigLocation:
        if (hasPath(SNAP_USER_COMMON))
            return getPath(SNAP_USER_COMMON) + configLoc;
        else
            return QDir::homePath() + configLoc;
    case AppConfigLocation:
        return SnapStandardPaths::writableLocation(DataLocation) + appName;
    case CommonAppConfigLocation:
        return SnapStandardPaths::writableLocation(DataLocation) + appName;
    default:
        break;
    }
    return QString();
}

QStringList SnapStandardPaths::standardLocations(SnapStandardPaths::StandardLocation location)
{
    QStringList dirs;
    switch (location) {
    case HomeLocation:
        if (hasPath(SNAP_USER_DATA)) {
            dirs << getPath(SNAP_USER_DATA);
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        }
        break;
    case CommonHomeLocation:
        if (hasPath(SNAP_USER_COMMON)) {
            dirs << getPath(SNAP_USER_COMMON);
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        }
        break;
    case CacheLocation:
        if (hasPath(SNAP_USER_DATA)) {
            dirs << getPath(SNAP_USER_DATA) + QStringLiteral("/.cache");
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
        }
        break;
    case CommonCacheLocation:
        if (hasPath(SNAP_USER_COMMON)) {
            dirs << getPath(SNAP_USER_COMMON) + QStringLiteral("/.cache");
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
        }
        break;
    case AppCacheLocation:
        if (hasPath(SNAP_USER_DATA)) {
            dirs << getPath(SNAP_USER_DATA) + QStringLiteral("/.cache/%1").arg(QCoreApplication::applicationName());
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
        }
        break;
    case CommonAppCacheLocation:
        if (hasPath(SNAP_USER_COMMON)) {
            dirs << getPath(SNAP_USER_COMMON) + QStringLiteral("/.cache/%1").arg(QCoreApplication::applicationName());
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
        }
        break;
    case DataLocation:
        if (hasPath(SNAP_USER_DATA) && hasPath(SNAP_DATA)) {
            dirs << getPath(SNAP_USER_DATA) + QStringLiteral("/.local/share");
            dirs << getPath(SNAP_DATA);
            dirs << getPath(SNAP) + QStringLiteral("/usr/share");
            dirs << getPath(SNAP) + QStringLiteral("/usr/local/share");
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::DataLocation);
        }
        break;
    case CommonDataLocation:
        if (hasPath(SNAP_USER_COMMON) && hasPath(SNAP_COMMON)) {
            dirs << getPath(SNAP_USER_COMMON) + QStringLiteral("/.local/share");
            dirs << getPath(SNAP_COMMON);
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::DataLocation);
        }
        break;
    case AppDataLocation:
        if (hasPath(SNAP_USER_DATA) && hasPath(SNAP_DATA)) {
            dirs << getPath(SNAP_USER_DATA) + QStringLiteral("/.local/share/%1").arg(QCoreApplication::applicationName());
            dirs << getPath(SNAP_DATA) + QStringLiteral("/%1").arg(QCoreApplication::applicationName());
            dirs << getPath(SNAP) + QStringLiteral("/usr/share/%1").arg(QCoreApplication::applicationName());
            dirs << getPath(SNAP) + QStringLiteral("/usr/local/share/%1").arg(QCoreApplication::applicationName());
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
        }
        break;
    case CommonAppDataLocation:
        if (hasPath(SNAP_USER_COMMON) && hasPath(SNAP_COMMON)) {
            dirs << getPath(SNAP_USER_COMMON) + QStringLiteral("/.local/share/%1").arg(QCoreApplication::applicationName());
            dirs << getPath(SNAP_COMMON) + QStringLiteral("/%1").arg(QCoreApplication::applicationName());
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
        }
        break;
    case ConfigLocation:
        if (hasPath(SNAP_USER_DATA)) {
            dirs << getPath(SNAP_USER_DATA) + QStringLiteral("/.config");
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
        }
        break;
    case CommonConfigLocation:
        if (hasPath(SNAP_USER_COMMON)) {
            dirs << getPath(SNAP_USER_COMMON) + QStringLiteral("/.config");
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
        }
        break;
    case AppConfigLocation:
        if (hasPath(SNAP_USER_DATA)) {
            dirs << getPath(SNAP_USER_DATA) + QStringLiteral("/.config/%1").arg(QCoreApplication::applicationName());
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
        }
        break;
    case CommonAppConfigLocation:
        if (hasPath(SNAP_USER_COMMON)) {
            dirs << getPath(SNAP_USER_COMMON) + QStringLiteral("/.config/%1").arg(QCoreApplication::applicationName());
        } else {
            dirs << QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
        }
        break;
    }
    return dirs;
}

void SnapStandardPaths::setTestModeEnabled(bool testMode)
{
    ssp_testMode = testMode;
}

bool SnapStandardPaths::isTestModeEnabled()
{
    return ssp_testMode;
}

