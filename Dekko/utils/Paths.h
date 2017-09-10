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
#ifndef PATHS_H
#define PATHS_H

#include <QObject>
#include <QtQuick>
#include <QQmlEngine>
#include <QJSEngine>

class Paths : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cachePath READ cachePath CONSTANT)
    Q_PROPERTY(QString configPath READ configPath CONSTANT)

    Q_ENUMS(ActionIcon)
    Q_ENUMS(UserScript)

public:
    explicit Paths(QObject *parent = 0);

    enum ActionIcon {
        NoIcon,AccountIcon,AttachmentIcon,ContextMenuIcon,DeleteIcon,DraftIcon,EditIcon,
        FolderIcon,ImportantIcon,CombineInboxIcon,InboxIcon,JunkIcon,MailForwardIcon,MailForwardedIcon,
        MailReadIcon,MailRepliedIcon,MailRepliedAllIcon,MailReplyIcon,MailReplyAllIcon,
        MailUnreadIcon,NavigationMenuIcon,NewContactIcon,UnStarredIcon,NoteIcon,NoneSelectedIcon,
        SearchIcon,SelectIcon,SendIcon,SentIcon,StarredIcon,SyncIcon,TagsIcon,ViewFullScreenIcon,
        CalendarTodayIcon,FilterIcon,TimerIcon,ViewListSymbolic,TickIcon,UndoIcon,CopyIcon,
        CutIcon,PasteIcon,ShareIcon,BrowserIcon
    };

    enum UserScript {
        CidQueryScript,FakeScript,FontScript,
        OverFlowScript,ViewportScript,ZoomScript
    };

    static QString iconUrl(const ActionIcon icon, bool prefix = true);
    // QML use only as it prepends qrc:/// to the icon url instead of :/
    Q_INVOKABLE QUrl actionIconUrl(const ActionIcon icon);
    Q_INVOKABLE QString userscript(const UserScript script);
    Q_INVOKABLE QString standardHomeLocation() {
        return Paths::homeLocation();
    }
    Q_INVOKABLE QString mimeIconForUrl(const QString &url);
    Q_INVOKABLE QString mimeIconForMimeType(const QString &mimeType);

    QString cachePath() const;
    QString configPath() const;
    QString cacheFileLocation(const QString &fileName);
    QString configFileLocation(const QString &fileName);
    static QString standardCacheLocation();
    static QString standardConfigLocation();
    static QString standardDataLocation();
    static QString homeLocation();
    static QString cacheLocationForFile(const QString & fileName);
    static QString configLocationForFile(const QString & fileName);
    static QString dataLocationForFile(const QString & fileName);
    static bool checkForStaleLockFile(QLockFile **lockFile, QString &filePath, QString &errorMessage);
    static QString findProviderFile();
    static QString providerIconForDomain(const QString &domain);

    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

signals:

public slots:

private:
    QString findUserScript(const QString &scriptName);

    class MimeTypeCache {
    public:
        explicit MimeTypeCache(void);

        void mapIconToMimeType(const QString &mimeType, const QString &icon);
        QString getIconForMimeType(const QString &type);
        QMimeType getMimeTypeForFile(const QString &path);
        QMimeType getMimeTypeForName(const QString &name);

    private:
        QMimeDatabase mimeDb;
        QHash<QString, QString> iconForMimetype;
    };
    static MimeTypeCache s_mimeCache;
};

#endif // PATHS_H
