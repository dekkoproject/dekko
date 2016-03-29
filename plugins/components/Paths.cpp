#include "Paths.h"
#include <QCoreApplication>

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

QObject *Paths::factory(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine);
    Paths *paths = new Paths(reinterpret_cast<QObject*>(engine));
    return paths;
}

