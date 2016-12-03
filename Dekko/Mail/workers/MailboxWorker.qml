/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

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
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Mail 1.0

AppListener {

    waitFor: [MailboxStore.listenerId]

    property string mailboxPickerUrl: ""

    Filter {
        type: MessageKeys.resetFolderLists
        onDispatched: {
            MailboxStore.smartFolders.reset()
            MailboxStore.standardFolders.reset()
        }
    }
    Filter {
        type: MailboxKeys.emptyTrash
        onDispatched: {
            Log.logInfo("MailboxWorker::emptyTrash", "Emptying trash folder. This cannot be undone!")
            Client.emptyTrash()
        }
    }
    Filter {
        type: MailboxKeys.markStandardFoldersRead
        onDispatched: {
            Log.logInfo("MailboxWorker::markStandardFoldersRead", "Marking unified standard folder as read")
            Client.markStandardFolderRead(message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.markStandardFolderRead
        onDispatched: {
            Log.logInfo("MailboxWorker::markStandardFolderRead", "Marking standard folder for account id: %1 as read".arg(message.accountId))
            Client.markStandardFolderRead(message.accountId, message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.syncStandardFolders
        onDispatched: {
            Log.logInfo("MailboxWorker::syncStandardFolders", "Syncing standard folder")
            Client.syncStandardFolder(message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.syncStandardFolder
        onDispatched: {
            Log.logInfo("MailboxWorker::syncStandardFolder", "Syncing folder for account: %1".arg(message.accountId))
            Client.syncStandardFolder(message.accountId, message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.syncFolder
        onDispatched: {
            Log.logInfo("MailboxWorker::syncFolder", "Syncing folder for account: %1".arg(message.accountId))
            Client.syncFolder(message.accountId, message.folderId)
        }
    }
    AppScript {
        property string pickerId: "move-msg-picker"
        runWhen: MailboxKeys.moveMessage
        script: {
            Log.logInfo("MailboxWorker::moveMessage", "Opening folder picker.")
            ViewActions.pushToStageArea(ViewKeys.messageListStack,
                                        mailboxPickerUrl,
                                        {
                                            pickerId: pickerId,
                                            accountId: message.accountId
                                        })
            once(MailboxKeys.folderSelected, function (result) {
                Log.logInfo("MailboxWorker::moveMessage", "Folder selected")
                if (result.pickerId !== pickerId) {
                    return;
                }
                if (result.folderType === Folder.StandardFolder) {
                    d.moveConfig = {specialUse: false, id: message.msgId, folder: result.folderId}
                } else {
                    d.moveConfig = {specialUse: true, id: message.msgId, folder: result.folderType}
                }
                ViewActions.popStageArea(ViewKeys.messageListStack)
                Log.logInfo("MailboxWorker::moveMessage", "Ordering the mail squirrels to take this message somewhere else")
                d.delayMoveTimer.start()
            })
            once(MailboxKeys.moveMessageCancelled, function (msg) {
                ViewActions.popStageArea(ViewKeys.messageListStack)
                exit.bind(this, 0)
            })
        }

    }

    QtObject {
        id: d
        property var moveConfig
        property Timer delayMoveTimer: Timer {
            interval:300
            repeat: false
            onTriggered: {
                Log.logInfo("MailboxWorker::moveMessage", "Ok the squirrels have been released! Message now in transit")
                if(d.moveConfig["specialUse"]) {
                    Client.moveToStandardFolder(d.moveConfig["id"], d.moveConfig["folder"])
                } else {
                    Client.moveToFolder(d.moveConfig["id"], d.moveConfig["folder"])
                }
                ViewActions.orderSimpleToast(qsTr("Message moved"))
            }
        }
    }
}
