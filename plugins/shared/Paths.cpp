#include "Paths.h"
#include <QCoreApplication>
#include <QLockFile>

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
        iconName = QStringLiteral("new-contact.svg");
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
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation).append(
                QStringLiteral("/%1").arg(qApp->organizationName()));
}

QString Paths::standardConfigLocation()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation).append(
                QStringLiteral("/%1").arg(qApp->organizationName()));
}

QString Paths::cacheLocationForFile(const QString &fileName)
{
    return Paths::standardCacheLocation().append(QStringLiteral("/%1").arg(fileName));
}

QString Paths::configLocationForFile(const QString &fileName)
{
    return Paths::standardConfigLocation().append(QStringLiteral("/%1").arg(fileName));
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
    // sanity check
    if (script.isEmpty()) {
        qFatal("Userscript: %s does not exist at any of the standard paths!", qPrintable(scriptName));
    }
    return QStringLiteral("file://%1").arg(script);
}
