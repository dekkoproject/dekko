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
pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail 1.0
import "../../actions"
import "../../actions/messaging"
import "../../actions/logging"
import "../../actions/views"

/*!
*
* MailboxStore API
*
* MailboxStore maintains the state and models for the mailboxes of each account.
*
* @ingroup qml_stores
*/
BaseMessagingStore {
    id: mboxStore

    // We only want to expose the actual models.
    property alias standardFoldersModel: stdFolders.children
    property alias smartFoldersModel: smartFolders.children

    MessageFilterCollection {
        id: stdFolders
        filter: MessageFilterCollection.StandardFolders
    }
    MessageFilterCollection {
        id: smartFolders
        filter: MessageFilterCollection.SmartFolders
    }

    Filter {
        type: MessageKeys.resetFolderLists
        onDispatched: {
            smartFolders.reset()
            stdFolders.reset()
        }
    }
    Filter {
        type: MailboxKeys.emptyTrash
        onDispatched: {
            Log.logInfo("MailboxStore::emptyTrash", "Emptying trash folder. This cannot be undone!")
            Client.emptyTrash()
        }
    }

    Filter {
        type: MailboxKeys.markStandardFoldersRead
        onDispatched: {
            Log.logInfo("MailboxStore::markStandardFoldersRead", "Marking unified standard folder as read")
            Client.markStandardFolderRead(message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.markStandardFolderRead
        onDispatched: {
            Log.logInfo("MailboxStore::markStandardFolderRead", "Marking standard folder for account id: %1 as read".arg(message.accountId))
            Client.markStandardFolderRead(message.accountId, message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.syncStandardFolders
        onDispatched: {
            Log.logInfo("MailboxStore::syncStandardFolders", "Syncing standard folder")
            Client.syncStandardFolder(message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.syncStandardFolder
        onDispatched: {
            Log.logInfo("MailboxStore::syncStandardFolder", "Syncing folder for account: %1".arg(message.accountId))
            Client.syncStandardFolder(message.accountId, message.standardFolder)
        }
    }

    Filter {
        type: MailboxKeys.syncFolder
        onDispatched: {
            Log.logInfo("MailboxStore::syncFolder", "Syncing folder for account: %1".arg(message.accountId))
            Client.syncFolder(message.accountId, message.folderId)
        }
    }

    AppScript {
        property string pickerId: "move-msg-picker"
        runWhen: MailboxKeys.moveMessage
        script: {
            ViewActions.pushToStageArea(ViewKeys.messageListStack,
                                        "qrc:/qml/views/MailboxPickerPage.qml",
                                        {
                                            pickerId: pickerId,
                                            accountId: message.accountId
                                        })
            once(MailboxKeys.folderSelected, function (result) {
                if (result.pickerId !== pickerId) {
                    return;
                }
                if (result.folderType === Folder.StandardFolder) {
//                    Client.moveToFolder(message.msgId, result.folderId)
                    d.moveConfig = {specialUse: false, id: message.msgId, folder: result.folderId}
                } else {
//                    Client.moveToStandardFolder(message.msgId, result.folderType)
                    d.moveConfig = {specialUse: true, id: message.msgId, folder: result.folderType}
                }
                d.delayMoveTimer.start()
            })
            once(MailboxKeys.moveMessageCancelled, exit.bind(this, 0))
        }

    }

    QtObject {
        id: d

        property var moveConfig
        property Timer delayMoveTimer: Timer {
            interval:300
            repeat: false
            onTriggered: {
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
