import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Accounts 1.0
import Dekko.Components 1.0
import "../../actions/composer"
import "../../stores/composer"
import "../../constants"
import "../components"

FocusScope {
    id: root
    width: parent.width
    height: state === "collapsed" ? from.height : from.height + col.height
    clip: true

    Item {
        id: from
        height: units.gu(4)
        width: parent.width
        StretchRow {
            anchors {
                leftMargin: Style.smallSpacing
                fill: parent
                rightMargin: units.gu(1.75)
            }
            spacing: Style.defaultSpacing

            Label {
                id: type
                anchors {
                    left: parent.left
                    top: parent.top
                    leftMargin: Style.smallSpacing
                    topMargin: Style.smallSpacing
                }
                text: qsTr("From:")
            }

            Stretcher {

                UbuntuShape {
                    id: delegate
                    aspect: UbuntuShape.Flat
                    color: Qt.rgba(0, 0, 0, 0.05)
                    radius: "small"
                    height: units.gu(3)
                    width: inner_avatar.width + label.width + units.gu(1.5)
                    implicitHeight: height
                    implicitWidth: width
                    visible: ComposerStore.hasValidIdentity
                    anchors {
                        left: parent.left
                        top: parent.top
                        topMargin: units.gu(0.5)
                    }

                    Avatar {
                        id: inner_avatar
                        width: height
                        anchors {
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                            margins: units.dp(1)
                        }
                        name: ComposerStore.identity.name
                        initials: ComposerStore.identity.initials
                        email: ComposerStore.identity.email
                        fontSize: "x-small"
                        validContact: true
                    }

                    Label {
                        id: label
                        anchors {
                            left: inner_avatar.right
                            leftMargin: units.gu(0.5)
                            verticalCenter: parent.verticalCenter
                        }
                        text: ComposerStore.identity.name
                    }
                }
            }
            Icon {
                height: units.gu(2.25)
                width: height
                anchors {
                    top: parent.top
                    topMargin: Style.smallSpacing
                }
                name: root.state === "collapsed" ? "down" : "up"
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: root.state === "collapsed" ? root.state = "expanded" : root.state = "collapsed"
        }
    }

    Column {
        id: col
        anchors {
            left: parent.left
            right: parent.right
            top: from.bottom
        }

        Repeater {
            model: ComposerStore.identitiesModel
            delegate: ListItem {
                id: d
                property var config: model.outgoing
                height: layout.height
                anchors {
                    left: parent.left
                    right: parent.right
                }
                Rectangle {
                    anchors.fill: parent
                    visible: ComposerStore.identityIndex === model.index
                    color: Qt.rgba(0, 0, 0, 0.05)
                }
                ListItemLayout {
                    id: layout
                    height: units.gu(5.5)
                    title.text: d.config.name
                    subtitle.text: d.config.email

                    Avatar {
                        height: units.gu(4.5)
                        width: height
                        name: d.config.name
                        initials: d.config.initials
                        email: d.config.email
                        fontSize: "medium"
                        validContact: true
                        SlotsLayout.position: SlotsLayout.Leading
                    }
                }
                onClicked: {
                    root.state = "collapsed"
                    ComposerActions.setIdentity(model.index)
                }
            }
        }
    }
    state: "collapsed"
    states: [
        State {
            name: "collapsed"
        },
        State {
            name: "expanded"
        }
    ]
    Behavior on height {
        UbuntuNumberAnimation{}
    }

    Line {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}

