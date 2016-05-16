import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import Dekko.Mail 1.0
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
                    SenderIdentityField {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }

                    RecipientField {
                        recipientType: RecipientType.To
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    RecipientField {
                        visible: ComposerStore.showCC
                        recipientType: RecipientType.Cc
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }
                    RecipientField {
                        visible: ComposerStore.showBCC
                        recipientType: RecipientType.Bcc
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
                    placeholderText: qsTr("Enter message")
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
            }
        }
    }
}

