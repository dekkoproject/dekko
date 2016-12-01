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
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Settings 1.0
import "../components"
import "../../constants"
import "../utils/UiUtils.js" as UiUtils

ListItem {
    id: h
    property var msg
    property bool detailsVisible: false
    width: parent.width
    height: defaultLayout.height
    divider.visible: false

    signal showDetails()

    ListItemLayout {
        id: defaultLayout
        title.text: msg.from.name
        title.elide: Text.ElideRight
        title.wrapMode: Text.NoWrap
        subtitle.text: msg.toRecipientsString
        subtitle.elide: Text.ElideRight
        subtitle.wrapMode: Text.NoWrap
        summary.text: detailsVisible ? qsTr("Hide details") : qsTr("View details")
        summary.color: UbuntuColors.blue
        // Hack to reparent the mousearea to the summary label.
        // so we can make it clickable.
        Component.onCompleted: ma.parent = defaultLayout.summary

        MouseArea {
            id: ma
            parent: defaultLayout.summary
            anchors.fill: parent
            onClicked: h.showDetails()
        }

        Avatar {
            id: avatar
            name: msg.from.name
            initials: msg.from.initials
            email: msg.from.address
            validContact: true
            fontSize: "large"
            SlotsLayout.position: SlotsLayout.Leading
        }

        Item {
            id: inner_infoCol

            property int iconsVerticalSpacing: Style.smallSpacing
            property int iconsHorizontalSpacing: Style.defaultSpacing

            height: inner_timeLabel.height + Style.smallSpacing + ctxt.height
            width: Math.max(inner_timeLabel.width, (ctxt.width + rply.width) - units.gu(3))
            SlotsLayout.overrideVerticalPositioning: true

            Label {
                id: inner_timeLabel
                y: defaultLayout.mainSlot.y + defaultLayout.title.y
                   + defaultLayout.title.baselineOffset - baselineOffset
                anchors.right: parent.right
                text: msg ? msg.prettyDate : ""
                fontSize: "small"
            }

            HeaderButton {
                id: ctxt
                anchors {
                    right: parent.right
                    rightMargin: -units.gu(1.5)
                    top: inner_timeLabel.top
                    topMargin: parent.iconsVerticalSpacing
                }
                height: units.gu(6)
                width: units.gu(4.5)
                iconSize: units.gu(2.25)
                iconColor: UbuntuColors.ash
                action: Action {
                    iconSource: Paths.actionIconUrl(Icons.ContextMenuIcon)
                    onTriggered: PopupUtils.open(Qt.resolvedUrl("../popovers/MessageActionPopover.qml"), ctxt, {msg: msg})
                }
            }
            HeaderButton {
                id: rply
                anchors {
                    right: ctxt.left
                    rightMargin: -units.gu(0.5)
                    top: inner_timeLabel.top
                    topMargin: parent.iconsVerticalSpacing
                }
                height: units.gu(6)
                width: units.gu(4.5)
                iconSize: units.gu(2.25)
                iconColor: UbuntuColors.ash
                action: Action {
                    iconSource: Paths.actionIconUrl(Icons.MailReplyIcon)
                    onTriggered: ComposerActions.respondToMessage(msg.isListPost ? SubmissionManager.ReplyList : SubmissionManager.Reply, msg.messageId)
                }
            }
        }
    }
}

