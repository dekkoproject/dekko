#ifndef PATHS_H
#define PATHS_H

#include <QObject>
#include <QStandardPaths>
#include <QtQuick>
#include <QQmlEngine>
#include <QJSEngine>

class Paths : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cachePath READ cachePath CONSTANT)
    Q_PROPERTY(QString configPath READ configPath CONSTANT)

    Q_ENUMS(ActionIcon)

public:
    explicit Paths(QObject *parent = 0);

    enum ActionIcon {
        NoIcon,
        AttachmentIcon,
        ContextMenuIcon,
        DeleteIcon,
        DraftIcon,
        EditIcon,
        FolderIcon,
        ImportantIcon,
        CombineInboxIcon,
        InboxIcon,
        JunkIcon,
        MailForwardIcon,
        MailForwardedIcon,
        MailReadIcon,
        MailRepliedIcon,
        MailRepliedAllIcon,
        MailReplyIcon,
        MailUnreadIcon,
        NavigationMenuIcon,
        NewContactIcon,
        UnStarredIcon,
        NoteIcon,
        NoneSelectedIcon,
        SelectIcon,
        SendIcon,
        SentIcon,
        StarredIcon,
        TagsIcon,
        ViewFullScreenIcon
    };

    static QString iconUrl(const ActionIcon icon, bool prefix = true);
    // QML use only as it prepends qrc:/// to the icon url instead of :/
    Q_INVOKABLE QUrl actionIconUrl(const ActionIcon icon);

    QString cachePath() const;
    QString configPath() const;
    QString cacheFileLocation(const QString &fileName);
    QString configFileLocation(const QString &fileName);
    static QString standardCacheLocation();
    static QString standardConfigLocation();
    static QString cacheLocationForFile(const QString & fileName);
    static QString configLocationForFile(const QString & fileName);

    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

signals:

public slots:
};

#endif // PATHS_H
