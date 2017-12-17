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
#include "MailPlugin.h"

#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <MailServiceClient.h>
#include <FolderList.h>
#include <Folder.h>
#include <MessageFilterCollection.h>
#include <MessageSet.h>
#include <MessageList.h>
#include <MailboxSearch.h>
#include <MailAddress.h>
#include <SenderIdentities.h>
#include <MessageBuilder.h>
#include <SubmissionManager.h>
#include "qmlenums.h"

void MailPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Mail"));
    // @uri Dekko.Mail
    qmlRegisterType<Folder>(uri, 1, 0, "Folder");
    qmlRegisterType<FolderList>(uri, 1, 0, "FolderList");
    qmlRegisterType<MessageFilterCollection>(uri, 1, 0, "MessageFilterCollection");
    qmlRegisterType<MessageList>(uri, 1, 0, "MessageList");
    qmlRegisterType<MinimalMessage>(uri, 1, 0, "MinimalMessage");
    qmlRegisterType<MailAddress>(uri, 1, 0, "MailAddress");
    qmlRegisterType<Message>(uri, 1, 0, "Message");
    qmlRegisterType<MailboxSearch>(uri, 1, 0, "MailboxSearch");
    qmlRegisterType<SenderIdentities>(uri, 1, 0, "SenderIdentities");
    qmlRegisterType<MessageBuilder>(uri, 1, 0, "MessageBuilder");
    qmlRegisterType<SubmissionManager>(uri, 1, 0, "SubmissionManager");

    qmlRegisterUncreatableType<MessageSet>(uri, 1, 0, "MessageSet", "Cannot create MessageSet from QML, c++ only chap!");
    qmlRegisterUncreatableType<StandardFolderSet>(uri, 1, 0, "StandardFolderSet", "Cannot create MessageSet from QML, c++ only chap!");
    qmlRegisterUncreatableType<SmartFolderSet>(uri, 1, 0, "SmartFolderSet", "Cannot create SmartFolderSet from QML, c++ only chap!");
    qmlRegisterUncreatableType<SearchService>(uri, 1, 0, "SearchService", "Cannot create SearchService from QML, c++ only chap!");
    qmlRegisterSingletonType<Client>(uri, 1, 0, "Client", Client::factory);
    qmlRegisterUncreatableType<RecipientType>(uri, 1, 0, "RecipientType", "Cannot create an instance in QML");
}

void MailPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

