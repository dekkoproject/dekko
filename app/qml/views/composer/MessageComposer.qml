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
import Dekko.Components 1.0
import Dekko.Mail 1.0
import "../../constants"
import "../../actions/composer"
import "../../actions/views"
import "../../stores/composer"

FocusScope {
    anchors.fill: parent

    ScrollView {
        id: sv
        anchors.fill: parent
        Flickable {
            id: flicker
            anchors.fill: parent
            contentHeight: col.height
            Column {
                id: col
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                Column {
                    id: contentHeader
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    // TODO: On height changes of the fields of this column
                    // we need to make sure the input field remains visible above the OSK.
                    // Some kind of "makeMeVisible" function similar to how we do it for the MessageField
                    // Probably needs to be positioned based on the bottom of the field as thats
                    // where the input actually is.
                    SenderIdentityField {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }

                    RecipientField {
                        recipientType: RecipientType.To
                        recipientModel: ComposerStore.recipients.to
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    RecipientField {
                        visible: ComposerStore.showCC
                        recipientType: RecipientType.Cc
                        recipientModel: ComposerStore.recipients.cc
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    RecipientField {
                        visible: ComposerStore.showBCC
                        recipientType: RecipientType.Bcc
                        recipientModel: ComposerStore.recipients.bcc
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    SubjectField {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                }
                MessageField {
                    id: messageField
                    placeholderText: text !== "" ? "" : qsTr("Enter message")
                    onKeyEventOccurred: {
                        var targetY = -1
                        var p = getCursorPosition().y
                        var positionOnScreen = p - (flicker.contentY - contentHeader.height)
                        if (positionOnScreen < 0) {
                            targetY = contentHeader.height + p
                        } else if (positionOnScreen > flicker.height - units.gu(5)) {
                            targetY = (contentHeader.height + p) - (flicker.height - units.gu(5))
                        }
                        if (targetY >= 0 && targetY < flicker.contentHeight) {
                            flicker.contentY = targetY
                        }
                        flicker.returnToBounds()
                    }
                }
            }
            /* Place this Item on the bottom of the column
               mainly because by default our text area doesn't take up all remaining space due to using autosize
               and our logic for repositioning the view. So we use a mouse area to force active focus
               for the area that the TextArea doesn't cover
            */
            Item {
                anchors {
                    top: col.bottom
                    left: parent.left
                    right: parent.right
                }
                // Take up remaining visible space
                readonly property int composerHeight: (flicker.height - col.height)
                height: composerHeight > 0 ? composerHeight : 0
                MouseArea {
                    anchors.fill: parent
                    onClicked: messageField.__textArea.forceActiveFocus()
                }
                Label {
                    anchors {
                        left: parent.left
                        leftMargin: Style.defaultSpacing
                        top: parent.top
                        topMargin: Style.smallSpacing
                        right: parent.right
                        rightMargin: Style.defaultSpacing
                    }
                    color: UbuntuColors.ash
                    text: ComposerStore.currentSignature
                }
            }
        }
    }
}

