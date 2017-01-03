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
import Ubuntu.Components 1.3
import Dekko.Mail.Accounts 1.0
import Dekko.Mail 1.0
import Dekko.Components 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Settings 1.0
import Dekko.Mail.Stores.Settings 1.0
import "../components"

SettingsGroupPage {
    pageHeader.title: qsTr("Copies and Folders")

    property AccountConfig incoming: account.incoming
    property bool creatingStandardFolders: false

    function settingsChanged() {
        if (incoming.baseFolder !== basePath.text
            || account.specialUseFolderPath(Account.InboxFolder) !== inbox.text
            || account.specialUseFolderPath(Account.DraftsFolder) !== drafts.text
            || account.specialUseFolderPath(Account.SentFolder) !== sent.text
            || account.specialUseFolderPath(Account.JunkFolder) !== junk.text
            || account.specialUseFolderPath(Account.OutboxFolder) !== outbox.text
            || account.specialUseFolderPath(Account.TrashFolder) !== trash.text
            || ccIncludes.text !== mailPolicy.ccIncludes
            || ccIncludes.text !== mailPolicy.ccIncludes )
        {
            Log.logInfo("CopyFoldersGroup::determineIfSettingsChanged", "Settings have changed")
            return true
        } else {
            Log.logInfo("CopyFoldersGroup::determineIfSettingsChanged", "No changes")
            return false
        }
    }

    MailPolicy {
        id: mailPolicy
        accountId: SettingsStore.selectedAccount.accountId
    }

    AppListener {

        Filter {
            type: SettingsKeys.saveCurrentGroup
            onDispatched: {
                Log.logInfo("CopyFoldersGroup::saveCurrentGroup", "Saving current group")
                if (!settingsChanged()) {
                    return
                }
                incoming.baseFolder = basePath.text
                account.setSpecialUseFolder(Account.InboxFolder, inbox.text)
                account.setSpecialUseFolder(Account.DraftsFolder, drafts.text)
                account.setSpecialUseFolder(Account.SentFolder, sent.text)
                account.setSpecialUseFolder(Account.JunkFolder, junk.text)
                account.setSpecialUseFolder(Account.OutboxFolder, outbox.text)
                account.setSpecialUseFolder(Account.TrashFolder, trash.text)
                mailPolicy.ccIncludes = ccIncludes.text
                mailPolicy.bccIncludes = bccIncludes.text
                SettingsStore.settingsChanged = true
                Log.logInfo("CopyFoldersGroup::saveCurrentGroup", "Current group saved")
                SettingsActions.currentGroupSaved()
            }
        }

        Filter {
            type: SettingsKeys.folderPathPicked
            onDispatched: {
                switch (message.fieldId) {
                case "base":
                    basePath.text = message.path
                    break
                case "inbox":
                    inbox.text = message.path
                    break
                case "drafts":
                    drafts.text = message.path
                    break
                case "spam":
                    junk.text = message.path
                    break
                case "sent":
                    sent.text = message.path
                    break
                case "outbox":
                    outbox.text = message.path
                    break
                case "trash":
                    trash.text = message.path
                    break
                }
            }
        }
    }

    Connections {
        target: Client
        onStandardFoldersCreated: {
            creatingStandardFolders = false
        }
    }

    PageFlickable {
        margins: 0
        anchors.topMargin: units.gu(1)
        SectionHeader {
            text: qsTr("Standard folders")
        }
        Item {
            height: units.gu(5)
            width: parent.width
            visible: false
            Label {
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    margins: units.gu(2)
                }
                text: qsTr("Detect standard folders")
            }

            ActivityIndicator {
                running: creatingStandardFolders
                visible: running
                anchors {
                    right: detectButton.left
                    rightMargin: units.gu(1)
                    verticalCenter: detectButton.verticalCenter
                }
            }

            Button {
                id: detectButton
                color: UbuntuColors.green
                text: qsTr("Detect")
                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    margins: units.gu(2)
                }
                onClicked: {
                    SettingsActions.detectStandardFolders()
                    creatingStandardFolders = true
                }
            }
        }

        TitledTextField {
            id: basePath
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            title: qsTr("Base folder")
            text: incoming.baseFolder
            placeholderText: qsTr("Leave empty if you are unsure")
            trailingAction: Action {
                iconSource: Paths.actionIconUrl(Icons.InboxIcon)
                onTriggered: SettingsActions.pickFolder(account.id, "base")
            }
        }

        TitledTextField {
            id: inbox
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            title: qsTr("Inbox folder")
            text: account.specialUseFolderPath(Account.InboxFolder)
            trailingAction: Action {
                iconSource: Paths.actionIconUrl(Icons.InboxIcon)
                onTriggered: SettingsActions.pickFolder(account.id, "inbox")
            }
        }
        TitledTextField {
            id: drafts
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            title: qsTr("Drafts folder")
            text: account.specialUseFolderPath(Account.DraftsFolder)
            trailingAction: Action {
                iconSource: Paths.actionIconUrl(Icons.InboxIcon)
                onTriggered: SettingsActions.pickFolder(account.id, "drafts")
            }
        }
        TitledTextField {
            id: junk
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            title: qsTr("Spam folder")
            text: account.specialUseFolderPath(Account.JunkFolder)
            trailingAction: Action {
                iconSource: Paths.actionIconUrl(Icons.InboxIcon)
                onTriggered: SettingsActions.pickFolder(account.id, "spam")
            }
        }
        TitledTextField {
            id: sent
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            title: qsTr("Sent folder")
            text: account.specialUseFolderPath(Account.SentFolder)
            trailingAction: Action {
                iconSource: Paths.actionIconUrl(Icons.InboxIcon)
                onTriggered: SettingsActions.pickFolder(account.id, "sent")
            }
        }
        TitledTextField {
            id: outbox
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            title: qsTr("Outbox folder")
            text: account.specialUseFolderPath(Account.OutboxFolder)
            trailingAction: Action {
                iconSource: Paths.actionIconUrl(Icons.InboxIcon)
                onTriggered: SettingsActions.pickFolder(account.id, "outbox")
            }
        }
        TitledTextField {
            id: trash
            anchors {
                left: parent.left
                right: parent.right
                margins: units.gu(2)
            }
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            title: qsTr("Trash folder")
            text: account.specialUseFolderPath(Account.TrashFolder)
            trailingAction: Action {
                iconSource: Paths.actionIconUrl(Icons.InboxIcon)
                onTriggered: SettingsActions.pickFolder(account.id, "trash")
            }
        }

        SectionHeader {
            text: qsTr("Sending messages")
        }

//        TitledTextField {
//            id: ccIncludes
//            anchors {
//                left: parent.left
//                right: parent.right
//                margins: units.gu(2)
//            }
//            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
//            title: qsTr("Cc these email addresses:")
//            text: mailPolicy ? mailPolicy.ccIncludes : ""
//            placeholderText: qsTr("Comma separated list")
//        }

//        TitledTextField {
//            id: bccIncludes
//            anchors {
//                left: parent.left
//                right: parent.right
//                margins: units.gu(2)
//            }
//            inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
//            title: qsTr("Bcc these email addresses:")
//            text: mailPolicy ? mailPolicy.bccIncludes : ""
//            placeholderText: qsTr("Comma separated list")
//        }

//        SectionHeader {
//            text: qsTr("Draft messages")
//        }
//        SectionHeader {
//            text: qsTr("Push folders")
//        }
    }
}
