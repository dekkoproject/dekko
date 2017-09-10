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
#include "Paths.h"
#include <QCoreApplication>
#include <QLockFile>
#include <SnapStandardPaths.h>

Paths::Paths(QObject *parent) : QObject(parent)
{

}

QString Paths::iconUrl(const Paths::ActionIcon icon, bool prefix)
{
    QString iconName;
    switch(icon) {
    case NoIcon:
        break;
    case AttachmentIcon:
        iconName = QStringLiteral("attachment.svg");
        break;
    case AccountIcon:
        iconName = QStringLiteral("account.svg");
        break;
    case ContextMenuIcon:
        iconName = QStringLiteral("contextual-menu.svg");
        break;
    case DeleteIcon:
        iconName = QStringLiteral("delete.svg");
        break;
    case DraftIcon:
        iconName = QStringLiteral("draft.svg");
        break;
    case EditIcon:
        iconName = QStringLiteral("edit.svg");
        break;
    case FolderIcon:
        iconName = QStringLiteral("folder.svg");
        break;
    case ImportantIcon:
        iconName = QStringLiteral("important.svg");
        break;
    case CombineInboxIcon:
        iconName = QStringLiteral("inbox-all.svg");
        break;
    case InboxIcon:
        iconName = QStringLiteral("inbox.svg");
        break;
    case JunkIcon:
        iconName = QStringLiteral("junk.svg");
        break;
    case MailForwardIcon:
        iconName = QStringLiteral("mail-forward.svg");
        break;
    case MailForwardedIcon:
        iconName = QStringLiteral("mail-forwarded.svg");
        break;
    case MailReadIcon:
        iconName = QStringLiteral("mail-read.svg");
        break;
    case MailRepliedIcon:
        iconName = QStringLiteral("mail-replied-all.svg");
        break;
    case MailRepliedAllIcon:
        iconName = QStringLiteral("mail-replied.svg");
        break;
    case MailReplyAllIcon:
        iconName = QStringLiteral("mail-reply-all.svg");
        break;
    case MailReplyIcon:
        iconName = QStringLiteral("mail-reply.svg");
        break;
    case MailUnreadIcon:
        iconName = QStringLiteral("mail-unread.svg");
        break;
    case NavigationMenuIcon:
        iconName = QStringLiteral("navigation-menu.svg");
        break;
    case NewContactIcon:
        iconName = QStringLiteral("contact-new.svg");
        break;
    case UnStarredIcon:
        iconName = QStringLiteral("non-starred.svg");
        break;
    case NoteIcon:
        iconName = QStringLiteral("note.svg");
        break;
    case NoneSelectedIcon:
        iconName = QStringLiteral("select-none.svg");
        break;
    case SearchIcon:
        iconName = QStringLiteral("search.svg");
        break;
    case SelectIcon:
        iconName = QStringLiteral("select.svg");
        break;
    case SendIcon:
        iconName = QStringLiteral("send.svg");
        break;
    case SentIcon:
        iconName = QStringLiteral("sent.svg");
        break;
    case StarredIcon:
        iconName = QStringLiteral("starred.svg");
        break;
    case SyncIcon:
        iconName = QStringLiteral("sync.svg");
        break;
    case TagsIcon:
        iconName = QStringLiteral("tags.svg");
        break;
    case ViewFullScreenIcon:
        iconName = QStringLiteral("view-fullscreen.svg");
        break;
    case CalendarTodayIcon:
        iconName = QStringLiteral("calendar-today.svg");
        break;
    case FilterIcon:
        iconName = QStringLiteral("filters.svg");
        break;
    case TimerIcon:
        iconName = QStringLiteral("timer.svg");
        break;
    case ViewListSymbolic:
        iconName = QStringLiteral("view-list-symbolic.svg");
        break;
    case TickIcon:
        iconName = QStringLiteral("tick.svg");
        break;
    case UndoIcon:
        iconName = QStringLiteral("undo.svg");
        break;
    case CopyIcon:
        iconName = QStringLiteral("edit-copy.svg");
        break;
    case CutIcon:
        iconName = QStringLiteral("edit-cut.svg");
        break;
    case PasteIcon:
        iconName = QStringLiteral("edit-paste.svg");
        break;
    case ShareIcon:
        iconName = QStringLiteral("share.svg");
        break;
    case BrowserIcon:
        iconName = QStringLiteral("webbrowser-app-symbolic.svg");
        break;
    };

    if (!iconName.isEmpty()) {
        if (prefix)
            return QStringLiteral(":/actions/%1").arg(iconName);
        else
            return QStringLiteral("/actions/%1").arg(iconName);

    } else {
        qDebug() << "Unknown icon";
        return QString();
    }
}

QUrl Paths::actionIconUrl(const Paths::ActionIcon icon)
{
    return QStringLiteral("qrc://%1").arg(Paths::iconUrl(icon, false));
}

QString Paths::userscript(const Paths::UserScript script)
{
    switch (script) {
    case CidQueryScript:
        return findUserScript(QStringLiteral("add_cid_query.js"));
    case FakeScript:
        return findUserScript(QStringLiteral("fake_script.js"));
    case FontScript:
        return findUserScript(QStringLiteral("font_user_script.js"));
    case OverFlowScript:
        return findUserScript(QStringLiteral("hide_overflow.js"));
    case ViewportScript:
        return findUserScript(QStringLiteral("viewport_shim.js"));
    case ZoomScript:
        return findUserScript(QStringLiteral("zoom.js"));
    }
    return QString();
}

QString Paths::mimeIconForUrl(const QString &url)
{
    static const QString defaultIcon = QStringLiteral("file");
    QString ret;
    const QString path = QUrl(url).toLocalFile();
    const QFileInfo info(path);
    if (info.exists()) {
        const QMimeType type = s_mimeCache.getMimeTypeForFile(path);
        ret = s_mimeCache.getIconForMimeType(type.name());
        if (ret.isEmpty()) {
            ret = defaultIcon;
        }
    } else {
        ret = defaultIcon;
    }
    return QStringLiteral("qrc:///mimetypes/%1.svg").arg(ret);
}

QString Paths::mimeIconForMimeType(const QString &mimeType)
{
    qDebug() << "Getting icon for mime type: " << mimeType;
    QString ret;
    const QMimeType type = s_mimeCache.getMimeTypeForName(mimeType);
    qDebug() << "TYPENAME: " << type.name();
    ret = s_mimeCache.getIconForMimeType(type.name());
    if (ret.isEmpty()) {
        ret = QStringLiteral("file");
    }
    qDebug() << "ICON NAME: " << ret;
    return QStringLiteral("qrc:///mimetypes/%1.svg").arg(ret);
}

QString Paths::cachePath() const
{
    return Paths::standardCacheLocation();
}

QString Paths::configPath() const
{
    return Paths::standardConfigLocation();
}

QString Paths::cacheFileLocation(const QString &fileName)
{
    return Paths::cacheLocationForFile(fileName);
}

QString Paths::configFileLocation(const QString &fileName)
{
    return Paths::configLocationForFile(fileName);
}

QString Paths::standardCacheLocation()
{
    return SnapStandardPaths::writableLocation(SnapStandardPaths::AppCacheLocation);
}

QString Paths::standardConfigLocation()
{
    return SnapStandardPaths::writableLocation(SnapStandardPaths::AppConfigLocation);
}

QString Paths::standardDataLocation()
{
    return SnapStandardPaths::writableLocation(SnapStandardPaths::AppDataLocation);
}

QString Paths::homeLocation()
{
    return SnapStandardPaths::writableLocation(SnapStandardPaths::HomeLocation);
}

QString Paths::cacheLocationForFile(const QString &fileName)
{
    return Paths::standardCacheLocation().append(QStringLiteral("/%1").arg(fileName));
}

QString Paths::configLocationForFile(const QString &fileName)
{
    return Paths::standardConfigLocation().append(QStringLiteral("/%1").arg(fileName));
}

QString Paths::dataLocationForFile(const QString &fileName)
{
    return Paths::standardDataLocation().append(QStringLiteral("/%1").arg(fileName));
}

bool Paths::checkForStaleLockFile(QLockFile **lockFile, QString &filePath, QString &errorMessage)
{
    QDir dir(Paths::standardConfigLocation());
    if (!dir.exists() && !dir.mkpath(QStringLiteral("."))) {
        errorMessage = QStringLiteral("Cannot create directory: %1").arg(dir.path());
        return false;
    }

    // Reset to config directory for consistency; avoid depending on this behavior for paths
    if (QDir::setCurrent(dir.absolutePath()) && dir.isRelative()) {
        dir.setPath(QStringLiteral("."));
    }
    QLockFile *lock = new QLockFile(filePath);
    *lockFile = lock;
    lock->setStaleLockTime(0);
    if (!lock->tryLock()) {
        if (lock->error() == QLockFile::LockFailedError) {
            // This happens if a stale lock file exists and another process uses that PID.
            // Try removing the stale file, which will fail if a real process is holding a
            // file-level lock. A false error is more problematic than not locking properly
            // on corner-case systems.
            if (!lock->removeStaleLockFile() || !lock->tryLock()) {
                errorMessage = QStringLiteral("Configuration file is already in use");
                return false;
            } else
                qDebug() << "Removed stale lock file";
        } else {
            errorMessage = QStringLiteral("Cannot write configuration file (failed to acquire lock)");
            return false;
        }
    }
    return true;
}

QString Paths::findProviderFile()
{
    QString configFile;
    // TODO: make this configurable
    const QString filePath = QStringLiteral("/usr/share/dekko/configuration/serviceProviders.conf");
    if (configFile.isEmpty()) {
        QString desktopFile = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../../../dekko2.desktop"));
        if (QFile::exists(desktopFile)) {
            QDir clickRoot = QFileInfo(desktopFile).absoluteDir();
            QString myPath = clickRoot.absolutePath() + QLatin1Char('/') + filePath;
            if (QFile::exists(myPath)) {
                configFile = myPath;
            }
        }
    }

    if (configFile.isEmpty()) {
        const QString snapPath = QString(qgetenv("SNAP")) + QStringLiteral("/usr/share/dekko");
        if (QDir(snapPath).exists()) {
            QString myPath = snapPath + QLatin1Char('/') + filePath;
            if (QFile::exists(myPath)) {
                configFile = myPath;
            }
        }
    }

    if (!configFile.isEmpty()) {
        qDebug() << "Config file found";
        return configFile;
    }
    qDebug() <<" Config file not found";
    return QString();
}

QString Paths::providerIconForDomain(const QString &domain)
{
    QString icon;
    if (domain == QStringLiteral("gmail.com")) {
        icon = QStringLiteral("online-account-google.svg");
    } else if (domain == QStringLiteral("outlook.com")) {
        icon = QStringLiteral("online-account-outlook.svg");
    } else if (domain == QStringLiteral("yahoo.com")) {
        icon = QStringLiteral("online-account-yahoo.svg");
    } else if (domain == QStringLiteral("me.com")) {
        icon = QStringLiteral("online-account.svg");
    } else {
        icon = QStringLiteral("dekko-app-symbolic.svg");
    }
    return QStringLiteral("qrc:/provider/%1").arg(icon);
}

QObject *Paths::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    Paths *paths = new Paths(reinterpret_cast<QObject*>(engine));
    return paths;
}


QString Paths::findUserScript(const QString &scriptName)
{
    QString script;
    if (script.isEmpty()) {
        QString desktopFile = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../../../dekko.desktop"));
        if (QFile::exists(desktopFile)) {
            // we are a click so lets tidy up our manifest path and find qmlfile
            QDir clickRoot = QFileInfo(desktopFile).absoluteDir();
            QString myPath = clickRoot.absolutePath() + QLatin1String("/userscripts/") + scriptName;
            if (QFile::exists(myPath)) {
                script = myPath;
            }
        }
    }

    if (script.isEmpty()) {
        const QString snapPath = QString(qgetenv("SNAP")) + QStringLiteral("/usr/share/dekko");
        if (QDir(snapPath).exists()) {
            QString myPath = snapPath + QLatin1String("/userscripts/") + scriptName;
            if (QFile::exists(myPath)) {
                script = myPath;
            }
        }
    }
    // sanity check
    if (script.isEmpty()) {
        qFatal("Userscript: %s does not exist at any of the standard paths!", qPrintable(scriptName));
    }
    return QStringLiteral("file://%1").arg(script);
}

Paths::MimeTypeCache Paths::s_mimeCache;

Paths::MimeTypeCache::MimeTypeCache()
{
    iconForMimetype.reserve(100);
    mapIconToMimeType("image/png", "image");
    mapIconToMimeType("image/jpeg", "image");
    mapIconToMimeType("image/gif", "image");

    mapIconToMimeType("image/svg", "drawing");
    mapIconToMimeType("image/svg+xml", "drawing");
    mapIconToMimeType("application/vnd.oasis.opendocument.graphics", "drawing");

    mapIconToMimeType("audio/mpeg", "sound");
    mapIconToMimeType("audio/x-wav", "sound");
    mapIconToMimeType("audio/midi", "sound");

    mapIconToMimeType("video/mp4", "video");

    mapIconToMimeType("text/x-csrc", "code");
    mapIconToMimeType("text/x-chdr", "code");
    mapIconToMimeType("text/x-c++src", "code");
    mapIconToMimeType("text/x-c++hdr", "code");
    mapIconToMimeType("text/x-qml", "code");
    mapIconToMimeType("text/x-java", "code");
    mapIconToMimeType("text/css", "code");
    mapIconToMimeType("application/javascript", "code");
    mapIconToMimeType("text/x-diff", "code");
    mapIconToMimeType("text/x-patch", "code");

    mapIconToMimeType("application/xml", "xml");

    mapIconToMimeType("application/x-shellscript", "script");
    mapIconToMimeType("application/x-perl", "script");

    mapIconToMimeType("application/x-object", "binary");
    mapIconToMimeType("application/octet-stream", "binary");

    mapIconToMimeType("application/x-cd-image", "disk-image");

    mapIconToMimeType("application/zip", "archive");
    mapIconToMimeType("application/x-xz-compressed-tar", "archive");
    mapIconToMimeType("application/x-compressed-tar", "archive");
    mapIconToMimeType("application/x-rar", "archive");
    mapIconToMimeType("application/x-rpm", "archive");
    mapIconToMimeType("application/gzip", "archive");
    mapIconToMimeType("application/vnd.debian.binary-package", "archive");
    mapIconToMimeType("application/vnd.android.package-archive", "archive");
    mapIconToMimeType("application/x-7z-compressed", "archive");
    mapIconToMimeType("application/x-bzip-compressed-tar", "archive");

    mapIconToMimeType("text/x-makefile", "text");
    mapIconToMimeType("text/x-log", "text");
    mapIconToMimeType("text/x-theme", "text");
    mapIconToMimeType("text/csv", "text");
    mapIconToMimeType("text/plain", "text");
    mapIconToMimeType("text/vcard", "text");
    mapIconToMimeType("text/markdown", "text");
    mapIconToMimeType("application/json", "text");

    mapIconToMimeType("application/pdf", "pdf");

    mapIconToMimeType("application/vnd.oasis.opendocument.text", "document");
    mapIconToMimeType("application/vnd.openxmlformats-officedocument.wordprocessingml.document", "document");
    mapIconToMimeType("application/msword", "document");

    mapIconToMimeType("application/vnd.oasis.opendocument.spreadsheet", "spreadsheet");
    mapIconToMimeType("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "spreadsheet");
    mapIconToMimeType("application/vnd.ms-excel", "spreadsheet");
    mapIconToMimeType("application/ms-excel", "spreadsheet");

    mapIconToMimeType("application/vnd.oasis.opendocument.presentation", "slideshow");
    mapIconToMimeType("application/vnd.openxmlformats-officedocument.presentationml.presentation", "slideshow");
    mapIconToMimeType("application/vnd.ms-powerpoint", "slideshow");

    mapIconToMimeType("text/html", "webpage");

    mapIconToMimeType("application/sql", "database");
    mapIconToMimeType("application/x-sqlite3", "database");

    mapIconToMimeType("application/x-executable", "executable");
    mapIconToMimeType("application/x-ms-dos-executable", "executable");
}

void Paths::MimeTypeCache::mapIconToMimeType(const QString &mimeType, const QString &icon)
{
    iconForMimetype.insert(mimeType, icon);
}

QString Paths::MimeTypeCache::getIconForMimeType(const QString &type)
{
    return iconForMimetype.value(type);
}

QMimeType Paths::MimeTypeCache::getMimeTypeForFile(const QString &path)
{
    return mimeDb.mimeTypeForFile(path);
}

QMimeType Paths::MimeTypeCache::getMimeTypeForName(const QString &name)
{
    return mimeDb.mimeTypeForName(name);
}
