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
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail.Accounts 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Settings 1.0
import Dekko.Mail.Stores.Settings 1.0

AppListener {

    waitFor: [SettingsStore.listenerId]

    property url mailboxPickerUrl: ""

    Filter {
        type: SettingsKeys.updateMarkAsReadInterval
        onDispatched: {
            if (!PolicyManager.idValid(message.accountId)) {
                Log.logInfo("SettingsWorker::updateMarkAsReadInterval", "Invalid account id. doing nothing")
                return;
            }
            Log.logInfo("SettingsWorker::updateMarkAsReadInterval", "Updating interval to %1 for account: %2".arg(message.interval).arg(message.accountId))
            PolicyManager.mailPolicy(message.accountId).markAsReadInterval = message.interval
        }
    }

    Filter {
        type: SettingsKeys.updateMarkAsReadMode
        onDispatched: {
            if (!PolicyManager.idValid(message.accountId)) {
                Log.logInfo("SettingsWorker::updateMarkAsReadMode", "Invalid account id. doing nothing")
                return;
            }
            Log.logInfo("SettingsWorker::updateMarkAsReadMode", "Updating mode to %1 for account: %2".arg(message.mode).arg(message.accountId))
            PolicyManager.mailPolicy(message.accountId).markRead = message.mode
        }
    }

    Filter {
        type: SettingsKeys.openSettingsGroup
        onDispatched: {
            SettingsActions.saveCurrentGroup()
            SettingsStore.currentGroup = message.group
            if (SettingsStore.viewState.isSmallFF) {
                ViewActions.pushToStageArea(ViewKeys.settingsStack1, SettingsStore.currentGroup, {})
            } else {
                ViewActions.replaceTopStageAreaItem(ViewKeys.settingsStack2, SettingsStore.currentGroup, {})
            }
        }
    }

    Filter {
        type: SettingsKeys.setSelectedAccount
        property Component accountComponent: Component {
            Account {
                accountId: SettingsStore.selectedAccountId
            }
        }
        onDispatched: {
            SettingsStore.selectedAccountId = message.account.id
            SettingsStore.selectedAccount = accountComponent.createObject()
        }
    }

    Filter {
        type: SettingsKeys.switchSettingsGroupLocation
        onDispatched: {
            SettingsActions.saveCurrentGroup()
            ViewActions.popStageArea(message.stackKey)
            delaySwitch.start()
        }
        property Timer delaySwitch: Timer {
            interval: 50
            repeat: false
            onTriggered: {
                console.log("CurrentGroup", SettingsStore.currentGroup)
                SettingsActions.openSettingsGroup(SettingsStore.currentGroup)
            }
        }
    }

    Filter {
        type: SettingsKeys.saveSelectedAccount
        onDispatched: {
            SettingsStore.selectedAccount.save()
            ViewActions.orderSimpleToast(qsTr("Account saved"))
        }
    }

    Filter {
        type: SettingsKeys.detectStandardFolders
        onDispatched: {
            SettingsActions.saveCurrentGroup()
            SettingsActions.saveSelectedAccount()
            Client.createStandardFolders(SettingsStore.selectedAccount.id)
        }
    }
    Filter {
        type: SettingsKeys.createStandardFolders
        onDispatched: {
            Client.createStandardFolders(message.accountId)
        }
    }

    AppScript {
        property string pickerId: "settings-mbox-picker"
        property string fieldId: ""
        runWhen: SettingsKeys.pickFolder
        script: {
            Log.logInfo("MailboxWorker::moveMessage", "Opening folder picker.")
            fieldId = message.fieldId
            ViewActions.pushToStageArea(ViewKeys.settingsStack1,
                                        mailboxPickerUrl,
                                        {
                                            pickerId: pickerId,
                                            accountId: message.accountId
                                        })
            once(SettingsKeys.folderPicked, function (result) {
                Log.logInfo("MailboxWorker::moveMessage", "Folder selected")
                if (result.pickerId !== pickerId) {
                    return;
                }
                SettingsActions.folderPathPicked(fieldId, result.folder.path)
                ViewActions.popStageArea(ViewKeys.settingsStack1)
            })

            once(SettingsKeys.pickFolderCancelled, function (msg) {
                ViewActions.popStageArea(ViewKeys.settingsStack1)
                exit.bind(this, 0)
            })
        }

    }
}
