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
#ifndef IMAGEHELPER_H
#define IMAGEHELPER_H

#include <QObject>
#include <QtSvg/QSvgRenderer>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <QQmlParserStatus>
#include <QColor>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFuture>
#include <QFutureWatcher>
#include <Helpers.h>
#include <Paths.h>


class SvgToPngWorker;

class ImageHelper : public QObject, public QQmlPropertyValueSource, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_INTERFACES(QQmlPropertyValueSource)
    QML_WRITABLE_PROPERTY_WITH_SIGNAL(int, icon, refresh)
    QML_WRITABLE_PROPERTY_WITH_SIGNAL(int, size, refresh)
    QML_WRITABLE_PROPERTY_WITH_SIGNAL(qreal, verticalRatio, refresh)
    QML_WRITABLE_PROPERTY_WITH_SIGNAL(qreal, horizontalRatio, refresh)
    QML_WRITABLE_PROPERTY_WITH_SIGNAL(QColor, color, refresh)
    QML_WRITABLE_PROPERTY_WITH_SIGNAL(QString, text, refresh)
    QML_WRITABLE_PROPERTY(QString, gravatarEmail)
    Q_PROPERTY(QUrl gravatarUrl READ gravatarUrl WRITE setGravatarUrl NOTIFY gravatarUrlChanged)

public:
    explicit ImageHelper(QObject *parent = 0);
    ~ImageHelper();

    static void setBasePath(const QString &basePath);
    static void setCachePath(const QString &cachePath);
    virtual void classBegin();
    virtual void componentComplete();
    //This is where the magic happens in qml. When we
    // do something like
    // Image {
    //   height: 10
    //   width: 10
    //   ImageHelper on source {
    //   }
    // this property will get set to the source property of the image
    virtual void setTarget(const QQmlProperty &target);

    QUrl gravatarUrl() const;
    void setGravatarUrl(const QUrl &gravurl);
signals:
    void refresh();
    void gravatarUrlChanged();
    void gravatarImagePath(QUrl path);
protected slots:
    void handleRefresh();
    void finished(QNetworkReply *reply);
    void writeUrlToQmlProperty();

private:
    QNetworkAccessManager *m_nam;
    QQmlProperty m_property;
    static QString s_basePath;
    static QString s_cachePath;
    static QSvgRenderer s_renderer;
    bool m_ready;
    bool m_requestSent;
    bool m_alreadySeen;
    QUrl m_gravatarUrl;
    QFuture<QUrl> m_future;
    QFutureWatcher<QUrl> m_watcher;

};

class SvgToPngWorker : public QObject
{
    Q_OBJECT
public:
    SvgToPngWorker(QObject *parent, QString &basePath, QString &cachePath);

    QUrl convertSvgToPng(const int &size, const qreal &h, const qreal &v, const QColor &color, const QString &icon);

signals:
    void complete();
    void failed();
private:
    QString m_basePath;
    QString m_cachePath;
};

class GravatarWorker : public QObject
{
    Q_OBJECT
public:
    explicit GravatarWorker(QObject *parent = 0);

    QUrl saveGravatar(const QByteArray &reply, const QString &path);
};

#endif // IMAGEHELPER_H
