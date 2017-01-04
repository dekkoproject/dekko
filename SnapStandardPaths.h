#ifndef SNAPSTANDARDPATHS_H
#define SNAPSTANDARDPATHS_H

#include <QStringList>

class Q_CORE_EXPORT SnapStandardPaths
{
public:

    enum StandardLocation {
        HomeLocation,
        CommonHomeLocation,
        CacheLocation,
        CommonCacheLocation,
        AppCacheLocation,
        CommonAppCacheLocation,
        DataLocation,
        CommonDataLocation,
        AppDataLocation,
        CommonAppDataLocation,
        ConfigLocation,
        CommonConfigLocation,
        AppConfigLocation,
        CommonAppConfigLocation
    };

    enum LocateOption {
        LocateFile = 0x0,
        LocateDirectory = 0x1
    };
    Q_DECLARE_FLAGS(LocateOptions, LocateOption)

    static QString writableLocation(StandardLocation location);
    static QStringList standardLocations(StandardLocation location);

    static void setTestModeEnabled(bool testMode);
    static bool isTestModeEnabled();

private:
    // prevent construction
    SnapStandardPaths();
    ~SnapStandardPaths();
};

#endif // SNAPSTANDARDPATHS_H
