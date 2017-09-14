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
#include "ImageHelper.h"
#include <QCoreApplication>
#include <QUrl>
#include <QFile>
#include <QPainter>
#include <QPainterPath>
#include <QCryptographicHash>
#include <QDebug>
#include <QImage>
#include <QString>
#include <QStringBuilder>
#include <QDir>
#include <QResource>
#include <QtConcurrent/QtConcurrentRun>
#include <QQmlNetworkAccessManagerFactory>
#include <QFuture>
#include <QtQuick>
#include <Paths.h>


QString ImageHelper::s_basePath;
QString ImageHelper::s_cachePath;

ImageHelper::ImageHelper(QObject *parent) :
    QObject(parent), m_icon(Paths::ActionIcon::NoIcon),m_size(0), m_verticalRatio(1.0), m_horizontalRatio(1.0),
    m_color(Qt::transparent), m_nam(0), m_ready(false), m_requestSent(false), m_alreadySeen(false)
{
    connect(this, SIGNAL(refresh()), this, SLOT(handleRefresh()));

    if (s_basePath.isEmpty()) {
        // Our default base path is loading from qrc files. This can be overidden to point
        // to any allowed path.
        ImageHelper::s_basePath = QStringLiteral(":/actions");
    }
    if (s_cachePath.isEmpty()) {
        ImageHelper::s_cachePath = (Paths::standardCacheLocation() % "/.ImageCache");
    }

    connect(this, &ImageHelper::gravatarEmailChanged, [=]() {
        QByteArray hash = QCryptographicHash::hash((m_gravatarEmail.toUtf8()),QCryptographicHash::Md5).toHex();
        QString gravUrl = QStringLiteral("http://www.gravatar.com/avatar/%1?d=404&s=%2").arg(hash, QString::number(m_size));
        setGravatarUrl(QUrl(gravUrl));
    });
    connect(&m_watcher, SIGNAL(finished()), this, SLOT(writeUrlToQmlProperty()));
}

ImageHelper::~ImageHelper()
{
}

void ImageHelper::setBasePath(const QString &basePath)
{
    ImageHelper::s_basePath = basePath;
}

void ImageHelper::setCachePath(const QString &cachePath)
{
    ImageHelper::s_cachePath = cachePath;
}

void ImageHelper::classBegin()
{
    m_ready = false;
}

void ImageHelper::componentComplete()
{
    m_ready = true;
    emit refresh();
}

void ImageHelper::setTarget(const QQmlProperty &target)
{
    m_property = target;
    emit refresh();
}

QUrl ImageHelper::gravatarUrl() const
{
    return m_gravatarUrl;
}

void ImageHelper::setGravatarUrl(const QUrl &gravurl)
{
    if (gravurl.isEmpty() || !gravurl.isValid()) {
        return;
    }
    if (m_gravatarEmail.isEmpty()) {
        return;
    }
    if (m_gravatarUrl != gravurl) {
        m_alreadySeen = false;
    }
    if (m_size == 0 || m_alreadySeen) {
        return;
    }
    m_gravatarUrl = gravurl;
    m_alreadySeen = true;
    // TODO: THis isn't really scalable but we only have a few atm. Once we have some more it probably makes
    // sense to move these to a qsettings file and query groups.
    if (m_gravatarEmail.contains(QStringLiteral("ubports.com"))) {
        if (m_property.isValid() && m_property.isWritable()) {
            m_property.write(QUrl::fromLocalFile(QStringLiteral(":/provider/ubports-forum.jpg")));
            return;
        }
    }
    if (m_gravatarEmail.contains(QStringLiteral("launchpad.net"))) {
        if (m_property.isValid() && m_property.isWritable()) {
            m_property.write(QUrl::fromLocalFile(QStringLiteral(":/provider/lp_logo.png")));
            return;
        }
    }
    if (m_gravatarEmail.contains(QStringLiteral("plus.google.com"))) {
        if (m_property.isValid() && m_property.isWritable()) {
            m_property.write(QUrl::fromLocalFile(QStringLiteral(":/provider/Google_plus.png")));
            return;
        }
    }
    if (m_gravatarEmail.contains(QStringLiteral("calendar-notification@google.com"))) {
        if (m_property.isValid() && m_property.isWritable()) {
            m_property.write(QUrl::fromLocalFile(QStringLiteral(":/provider/Google_Calendar.png")));
            return;
        }
    }
    if (m_gravatarEmail.contains(QStringLiteral("ubuntu-sdk-build-bot"))) {
        if (m_property.isValid() && m_property.isWritable()) {
            m_property.write(QUrl::fromLocalFile(QStringLiteral(":/provider/ubuntu-sdk-build-bot.png")));
            return;
        }
    }
    QString avatarCache(s_cachePath % "/" % m_gravatarEmail % QString::number(m_size) % ".png");
    if (QFile::exists(avatarCache)) {
        if (m_property.isValid() && m_property.isWritable()) {
            m_property.write(QUrl::fromLocalFile(avatarCache));
        }
    } else {
        if (!m_requestSent) {
            m_nam = new QNetworkAccessManager(this);
            connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
            QNetworkRequest request(m_gravatarUrl);
            m_nam->get(request);
            m_requestSent = true;
        }
    }
}

void ImageHelper::handleRefresh()
{
    if (m_ready) {
        if ((Paths::ActionIcon)m_icon != Paths::ActionIcon::NoIcon && m_size > 0 && m_horizontalRatio > 0.0 && m_verticalRatio > 0.0) {
            // So this is quite an intensive task so we move it to a seperate thread to not starve the gui
            // benchmarking shows this greatly increases our load times in the message delegate, as this is a completely
            // asynchronous job and we actually only do the converting to png the once and just return the cached
            // version on each subsequent request.
            SvgToPngWorker *worker = new SvgToPngWorker(this, ImageHelper::s_basePath, ImageHelper::s_cachePath);
            m_future = QtConcurrent::run(worker, &SvgToPngWorker::convertSvgToPng,
                                                     m_size, m_horizontalRatio, m_verticalRatio,
                                                     m_color, Paths::iconUrl((Paths::ActionIcon)m_icon));
            m_watcher.setFuture(m_future);
        }
    }
}

void ImageHelper::finished(QNetworkReply *reply)
{
    QString avatarCache(s_cachePath % "/" % m_gravatarEmail % QString::number(m_size) % ".png");
    if (reply->error() == QNetworkReply::NoError) {
        GravatarWorker *worker = new GravatarWorker(this);
        m_future = QtConcurrent::run(worker, &GravatarWorker::saveGravatar, reply->readAll(), avatarCache);
        m_watcher.setFuture(m_future);
    } else {
        m_property.write(QUrl());
    }
    // let's just ignore anything other than a NoError as we don't need to handle it.
    reply->deleteLater();
    m_nam->deleteLater();
    return;
}

void ImageHelper::writeUrlToQmlProperty()
{
    QUrl url = m_future.result();
    if (!url.isValid() || url.isEmpty()) {
        return;
    }
    if (m_property.isValid() && m_property.isWritable()) {
        m_property.write(url);
    }
}

SvgToPngWorker::SvgToPngWorker(QObject *parent, QString &basePath, QString &cachePath) :
    QObject(parent), m_basePath(basePath), m_cachePath(cachePath)
{

}

QUrl SvgToPngWorker::convertSvgToPng(const int &size, const qreal &h, const qreal &v, const QColor &color, const QString &icon)
{
    QUrl url;
    if (!icon.isEmpty() && size > 0 && h > 0.0 && v > 0.0) {
        QImage image (size * h, size * v, QImage::Format_ARGB32);
        QString uri (icon
                     % "?color="  % (color.isValid() ? color.name() : QStringLiteral("none"))
                     % "&width="  % QString::number (image.width())
                     % "&height=" % QString::number (image.height()));
        QString hash (QCryptographicHash::hash(uri.toLocal8Bit(), QCryptographicHash::Md5).toHex());
        QString sourcePath = icon;
        QString cachedPath = QStringLiteral("%1/.ImageCache/%2.png").arg(Paths::standardCacheLocation(), hash);
        if(!QFile::exists(cachedPath)) {
            QPainter painter(&image);
            image.fill(Qt::transparent);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
            painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
            QSvgRenderer renderer;
            if (QFile::exists(sourcePath)) {
                renderer.load(sourcePath);
                if (renderer.isValid()) {
                    renderer.render(&painter);
                    if (color.isValid() && color.alpha() > 0) {
                        QColor tmp (color);
                        for (int x = 0; x < image.width(); x++) {
                            for (int y = 0; y < image.height(); y++) {
                                tmp.setAlpha(qAlpha (image.pixel(x, y)));
                                image.setPixel(x, y, tmp.rgba());
                            }
                        }
                    }
                    QDir().mkpath(m_cachePath);
                    image.save(cachedPath, "PNG", 0);
                    url = QUrl::fromLocalFile(cachedPath);
                }
            }
            else {
                qWarning() << ">>> SvgToPngWorker: Can't render" << sourcePath << ", no such file!";
            }
        }
        else {
            url = QUrl::fromLocalFile(cachedPath);
        }
    }
    return url;
}

GravatarWorker::GravatarWorker(QObject *parent) : QObject(parent)
{
}

QUrl GravatarWorker::saveGravatar(const QByteArray &reply, const QString &path)
{
    QImage *img = new QImage();
    img->loadFromData(reply, 0);
    if (img->isNull()) {
        qDebug() << ">>> GravatarWorker: Failed loading gravatar from reply";
        return QUrl();
    }
    if (img->save(path, "PNG", 0)) {
        return QUrl::fromLocalFile(path);
    }
    return QUrl();
}


