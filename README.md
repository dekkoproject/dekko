This is a QStandardPaths replacement for snap environments. 

Paths provided are:

| Location | Path |
| --- | --- |
| `SnapStandardPaths::HomeLocation` | `$SNAP_USER_DATA` |
| `SnapStandardPaths::CommonHomeLocation` | `$SNAP_USER_COMMON` |
| `SnapStandardPaths::CacheLocation` | `$SNAP_USER_DATA/.cache` |
| `SnapStandardPaths::CommonCacheLocation` | `$SNAP_USER_COMMON/.cache` |
| `SnapStandardPaths::AppCacheLocation` | `$SNAP_USER_DATA/.cache/<appname>` |
| `SnapStandardPaths::CommonAppCacheLocation` | `$SNAP_USER_COMMON/.cache/<appname>` |
| `SnapStandardPaths::DataLocation` | `$SNAP_USER_DATA/.local/share` , `$SNAP_DATA/usr/local/share` , `$SNAP_DATA/usr/share` |
| `SnapStandardPaths::CommonDataLocation` | `$SNAP_USER_COMMON/.local/share` , `$SNAP_COMMON` |
| `SnapStandardPaths::AppDataLocation` | `$SNAP_USER_DATA/.local/share/<appname>` , `$SNAP_DATA/usr/local/share/<appname>` , `$SNAP_DATA/usr/share/<appname>` |
| `SnapStandardPaths::CommonAppDataLocation` | `$SNAP_USER_COMMON/.local/share/<appname>` |
| `SnapStandardPaths::ConfigLocation` | `$SNAP_USER_DATA/.config` |
| `SnapStandardPaths::CommonConfigLocation` | `$SNAP_USER_COMMON/.config` |
| `SnapStandardPaths::AppConfigLocation` | `$SNAP_USER_DATA/.config/<appname>` |
| `SnapStandardPaths::CommonAppConfigLocation` | `$SNAP_USER_COMMON/.config/<appname>` |


If used in a non snap environment suitable paths will be returned using QStandardPaths