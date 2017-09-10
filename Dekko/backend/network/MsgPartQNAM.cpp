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
#include "MsgPartQNAM.h"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <qmailmessage.h>
#include "MsgPartReply.h"
#include "MsgReply.h"
#include "ForbiddenReply.h"

const QStringList Allowed::urlSchemes = QStringList() << QStringLiteral("dekko-msg")
                                                      << QStringLiteral("dekko-part")
                                                      << QStringLiteral("cid");
const QStringList Allowed::hosts = QStringList() << QStringLiteral("msg")
                                                 << QStringLiteral("www.gravatar.com")
                                                 << QStringLiteral("cdn.libravatar.org");

MsgPartQNAM::MsgPartQNAM(QObject *parent) : QNetworkAccessManager(parent)
{
}

MsgPartQNAM::~MsgPartQNAM()
{
}

QNetworkReply *MsgPartQNAM::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{

    qDebug() << "[MsgPartQNAM]" << __func__ << request.url();
    QString host = request.url().host();
    if (request.url().scheme() == QStringLiteral("cid")) {
        // Do nothing
    } else if (!Allowed::hosts.contains(host)) {
        qDebug() << "[MsgPartQNAM]" << __func__ << "Not in allowed hosts";
        return new ForbiddenReply(this);
    } else if (host != QStringLiteral("msg")) {
        // This is our avatar requests
        QNetworkRequest req(request);
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        return QNetworkAccessManager::createRequest(op, req, outgoingData);
    }
    QString scheme = request.url().scheme();
    if (!Allowed::urlSchemes.contains(scheme)) {
        qDebug() << "[MsgPartQNAM]" << __func__ << "Not in allowed schemes";
        return new ForbiddenReply(this, QString("%1 is not an allowed scheme").arg(scheme));
    }
    // Ok so this is now either a msg or cid part we are requesting
    QUrlQuery query(request.url());
    if (!query.hasQueryItem("messageId")) {
        qDebug() << "[MsgPartQNAM]" << __func__ << "Missing messageId";
        return new ForbiddenReply(this, QStringLiteral("Missing message id"));
    }
    QMailMessageId messageId(query.queryItemValue(QStringLiteral("messageId")).toInt());
    if (!messageId.isValid()) {
        qDebug() << "Invalid message id: " << messageId.toULongLong();
        return new ForbiddenReply(this, QStringLiteral("Invalid message id"));
    }

    if (scheme == QStringLiteral("cid")) {
        QString cidPath = request.url().path();
        return new MsgPartReply(this, messageId, cidPath);
    } else if (scheme == QStringLiteral("dekko-part")) { // implies we have a location qeury
        if (!query.hasQueryItem(QStringLiteral("location"))) {
            qDebug() << "dekko-part missing location query item";
            // TODO: should we try and determine at least a viewable part instead of bailing out?
            return new ForbiddenReply(this, QStringLiteral("Missing message part location"));
        }
        QString location = query.queryItemValue(QStringLiteral("location"));
        bool isPlainText = query.hasQueryItem(QStringLiteral("requestFormatting"));
        return new MsgPartReply(this, messageId, location, isPlainText, query.hasQueryItem(QStringLiteral("requestFormatting")));

    } else if (scheme == QStringLiteral("dekko-msg")) {
        // This is a non multipart message so we treat that a little different as it doesn't have parts
        return new MsgReply(this, messageId, query.hasQueryItem(QStringLiteral("requestFormatting")));
    } else {
        return new ForbiddenReply(this);
    }
}

