import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../utils/UiUtils.js" as UiUtils
import "../components"
import "../../actions/views"
import "../../stores/mail"

Flipable {
    id: flipable
    property var msg

    front: Item {
        id: inner_avatar
        anchors.fill: parent
        Component {
            id: component_inner_av
            UbuntuShape {
                id: avatarCircle
                anchors.fill: parent
                anchors.centerIn: parent
                aspect: UbuntuShape.Flat
                backgroundColor: msg ? UiUtils.getIconColor(msg.from.name) : "transparent"
                opacity: ma.containsMouse ? 0.8 : 1.0

                Label {
                    id: initialsLabel
                    color: "#FFFFFF"
                    text: msg ? msg.from.initials : ""
                    anchors.centerIn: parent
                    fontSize: "large"
                }
                Icon {
                    id: contactIcon
                    name: "contact"
                    anchors.centerIn: parent
                    height: parent.height - units.gu(2)
                    width: height
                    color: "white"
                    visible: !initialsLabel.text
                }
            }
        }
        Loader {
            id: loader_inner_av
            anchors.fill: parent
            active: !inner_name.visible
            sourceComponent: component_inner_av
        }
        UbuntuShapeGravatar {
            id: inner_name
            emailAddress: msg ? msg.from.address : ""
            clip: true
            anchors.fill: parent
            visible: status === Image.Ready
            opacity: ma.containsMouse ? 0.8 : 1.0
        }

        MouseArea {
            id: ma
            hoverEnabled: true
            anchors.fill: parent
            visible: flipable.state === "front"
            onClicked: {
                if (flipable.state === "front") {
                    ViewActions.pushToStageArea(ViewKeys.messageListStack, "qrc:///qml/views/ContactFilterView.qml",
                                                {
                                                    title: msg.from.name,
                                                    email: msg.from.address,
                                                    initials: msg.from.initials,
                                                    filterKey: msg.senderMsgKey
                                                }
                                                )
                }
            }
            // For some weird reason we can't just bind
            // avatarCircle to containsMouse. As during
            // pushing and popping another page to the stack
            // can cause multiple avatars with the search overaly visible.
            // It's mighty weird!!!
            // So instead we just manually show/hide when mouse enters/exits
            // Not ideal but it enforces the overlay is only visible when we want
            onEntered: {
                if (containsMouse) {
                    avatarSearchOverlay.visible = true
                }
            }
            onExited: {
                if (!containsMouse) {
                    avatarSearchOverlay.visible = false
                }
            }
        }

        UbuntuShape {
            id: avatarSearchOverlay
            anchors.fill: parent
            anchors.centerIn: parent
            aspect: UbuntuShape.Flat
            backgroundColor: UbuntuColors.slate
            opacity: 0.8
            visible: false
            CachedImage {
                width: parent.width - units.gu(2)
                height: width
                anchors.centerIn: parent
                name: Icons.SearchIcon
                color: "#ffffff"
            }
        }
    }

    back: Item {
        anchors.fill: parent
        ListItemWithActionsCheckBox {
            anchors.centerIn: parent
            checked: msg && msg.checked
        }
    }

    transform: Rotation {
        id: rotation
        origin.x: flipable.width/2
        origin.y: flipable.height/2
        axis.x: 0; axis.y: 1; axis.z: 0
    }

    state: MailStore.isInSelectionMode ? "back" : "front"

    states: [
        State {
            name: "front"
            PropertyChanges { target: rotation; angle: 0 }

        },
        State {
            name: "back"
            PropertyChanges { target: rotation; angle: 180 }
        }
    ]

    transitions: Transition {
        SequentialAnimation {
            PauseAnimation {
                duration: {
                    var result;
                    if (model.index > listView.selectionIndex) {
                        result = model.index - listView.selectionIndex
                    } else if ( model.index < listView.selectionIndex) {
                        result = listView.selectionIndex - model.index
                    }
                    if (result > 0 && result <= 20) {
                        return result * 100
                    } else {
                        return 0
                    }
                }
            }
            NumberAnimation { target: rotation; property: "angle"; duration: UbuntuAnimation.FastDuration }
        }
    }
}
