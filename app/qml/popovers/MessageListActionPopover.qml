import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Mail 1.0
import Dekko.Components 1.0
import "../components"

Popover {
    id: actionPopover
    property var msg

    Column {
        id: containerLayout
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        ListItem {
            height: mrl.height
            divider.visible: false
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: mrl
                height: units.gu(5)
                title.text: msg && msg.isRead ? qsTr("Mark as unread") : qsTr("Mark as read")

                CachedImage {
                    name: msg && msg.isRead ? Icons.MailUnreadIcon : Icons.MailReadIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                Client.markMessageRead(msg.messageId, !msg.isRead);
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: mil.height
            divider.visible: false
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: mil
                height: units.gu(5)
                title.text: msg && msg.isImportant ? qsTr("Mark as not important") : qsTr("Mark as important")

                CachedImage {
                    name: msg && msg.isImportant ? Icons.UnStarredIcon : Icons.StarredIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                Client.markMessageImportant(msg.messageId, !msg.isImportant)
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: msl.height
            divider.visible: true
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: msl
                height: units.gu(5)
                title.text: qsTr("Mark as spam")

                CachedImage {
                    name: Icons.JunkIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                dekko.showNotice("Mark as spam not implemented yet. Fix it before release!!!!")
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: tdl.height
            divider.visible: !done.visible
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: tdl
                height: units.gu(5)
                title.text: qsTr("To-do")

                CachedImage {
                    name: Icons.ViewListSymbolic
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }

                CachedImage {
                    name: Icons.TickIcon
                    height: units.gu(3)
                    width: height
                    visible: msg && msg.isTodo
                    color: UbuntuColors.green
                }
            }
            onClicked: {
                Client.markMessageTodo(msg.messageId, !msg.isTodo)
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            id: done
            height: doneLayout.height
            divider.visible: true
            visible: msg && msg.isTodo
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: doneLayout
                height: units.gu(5)
                title.text: qsTr("Done")

                CachedImage {
                    name: Icons.SelectIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                dekko.showNotice("Mark as Done not implemented yet. Fix it before release!!!!")
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: rplyl.height
            divider.visible: false
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: rplyl
                height: units.gu(5)
                title.text: qsTr("Reply")

                CachedImage {
                    name: Icons.MailRepliedIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                dekko.showNotice("Reply not implemented yet. Fix it before release!!!!")
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: ral.height
            divider.visible: false
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: ral
                height: units.gu(5)
                title.text: qsTr("Reply all")

                CachedImage {
                    name: Icons.MailRepliedAllIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                dekko.showNotice("Reply all not implemented yet. Fix it before release!!!!")
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: fwdl.height
            divider.visible: true
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: fwdl
                height: units.gu(5)
                title.text: qsTr("Forward")

                CachedImage {
                    name: Icons.MailForwardedIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                dekko.showNotice("Froward not implemented yet. Fix it before release!!!!")
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: ml.height
            divider.visible: false
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: ml
                height: units.gu(5)
                title.text: qsTr("Move")

                CachedImage {
                    name: Icons.InboxIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                dekko.showNotice("Move not implemented yet. Fix it before release!!!!")
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: rl.height
            divider.visible: false
            visible: msg && msg.canBeRestored
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: rl
                height: units.gu(5)
                title.text: qsTr("Restore to %1").arg(msg.previousFolderName)

                CachedImage {
                    name: Icons.UndoIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                Client.restoreMessage(msg.messageId)
                PopupUtils.close(actionPopover)
            }
        }
        ListItem {
            height: dl.height
            divider.visible: false
            color: UbuntuColors.porcelain
            ListItemLayout {
                id: dl
                height: units.gu(5)
                title.text: qsTr("Delete")

                CachedImage {
                    name: Icons.DeleteIcon
                    height: units.gu(3)
                    width: height
                    color: UbuntuColors.ash
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
            onClicked: {
                Client.deleteMessage(msg.messageId)
                PopupUtils.close(actionPopover)
            }
        }
    }
}
