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
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import Dekko.Mail 1.0
import "../components"
import "../../actions/views"
import "../../actions/messaging"
import "../../constants"

Item {
    id: header

    property var msg

    height: dekko.viewState.isSmallFF ? units.gu(6) : units.gu(9.075)
    anchors {
        left: parent.left
        right: parent.right
        top: parent.top
    }

    property bool showBack: dekko.viewState.isSmallFF

    HeaderButton {
        id: inner_button
        anchors {
            top: parent.top
            left: parent.left
        }
        visible: showBack

        height: units.gu(6)
        width: showBack ? units.gu(4.5) : 0
        action: Action {
            iconName: "back"
            onTriggered: {
                // Back button is only shown in the message list stack
                ViewActions.popStageArea(ViewKeys.messageListStack)
            }
        }
    }

    Label {
        id: fromLabel
        anchors {
            left: inner_button.right
            leftMargin: showBack ? 0 : Style.defaultSpacing
            top: parent.top
            topMargin: units.gu(1.5)
            right: starButton.left
            rightMargin: Style.smallSpacing
        }
        clip: true
        text: msg.subject
        elide: Text.ElideRight
        font.weight: Font.Light
        fontSize: "large"
    }

    RowLayout {
        anchors {
            left: fromLabel.left
            top: fromLabel.bottom
            topMargin: Style.smallSpacing
            right: starButton.left
            rightMargin: Style.smallSpacing
        }

        Label {
            id: subjectLabel
            text: msg.from.name
            clip: true
            elide: Text.ElideRight
            visible: !dekko.viewState.isSmallFF
            font.weight: Font.Normal

        }
    }

    HeaderButton {
        id: starButton
        anchors {
            right: deleteButton.left
            top: parent.top
        }
        height: units.gu(6)
        width: units.gu(4.5)
        iconSize: units.gu(2.25)
        iconColor: msg.isImportant ? "#f0e442" : "#888888"
        action: Action {
            iconSource: msg.isImportant ? Paths.actionIconUrl(Icons.StarredIcon) : Paths.actionIconUrl(Icons.UnStarredIcon)
            onTriggered: MessageActions.markMessageImportant(msg.messageId, !msg.isImportant)
        }
    }

    HeaderButton {
        id: deleteButton
        anchors {
            right: parent.right
            rightMargin: Style.smallSpacing
            top: parent.top
        }
        height: units.gu(6)
        width: units.gu(4.5)
        iconSize: units.gu(2.25)
        action: Action {
            iconName: "delete"
            shortcut: "Delete"
            onTriggered: {
                MessageActions.deleteMessage(msg.messageId)
            }
        }
    }

    Line {
        id: btmMargin
        anchors {
            left: parent.left
            right: parent.right
            bottom: header.bottom
        }
        visible: true
    }
}
