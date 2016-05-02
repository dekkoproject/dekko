import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import Dekko.Settings 1.0
import "../components"
import "../utils/UiUtils.js" as UiUtils

ListItem {
    id: h
    property var msg
    property bool detailsVisible: false
    width: parent.width
    height: gs.isCleanLayout ? cleanLayout.height + units.gu(1) : defaultLayout.height
    divider.visible: false

    signal showDetails()

    GlobalSettings {
        id: gs
        property bool isCleanLayout: data.messageview.style === "clean"
    }

    ListItemLayout {
        id: defaultLayout
        visible: !gs.isCleanLayout
        title.text: msg.from.name
        subtitle.text: msg.toRecipientsString
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

            property int iconsVerticalSpacing: units.gu(1)
            property int iconsHorizontalSpacing: units.gu(2)

            height: inner_timeLabel.height + units.gu(1) + ctxt.height
            width: Math.max(inner_timeLabel.width, ctxt.width + rply.width + units.gu(1))
            SlotsLayout.overrideVerticalPositioning: true

            Label {
                id: inner_timeLabel
                y: defaultLayout.mainSlot.y + defaultLayout.title.y
                   + defaultLayout.title.baselineOffset - baselineOffset
                anchors.right: parent.right
                text: msg ? msg.prettyDate : ""
                fontSize: "small"
            }

            CachedImage {
                id: ctxt
                height: units.gu(2.5)
                width: height
                anchors {
                    right: parent.right
                    top: inner_timeLabel.bottom
                    topMargin: parent.iconsVerticalSpacing
                }
                name: Icons.ContextMenuIcon
                color: UbuntuColors.ash
                AbstractButton {
                    anchors.fill: parent
                    onClicked: dekko.showNotice("Not implemented yet. Fix it before release!!!!")
                }
            }

            CachedImage {
                id: rply
                height: units.gu(2.5)
                width: visible ? height : 0
                anchors {
                    right: ctxt.left
                    rightMargin: parent.iconsHorizontalSpacing
                    top: inner_timeLabel.bottom
                    topMargin: parent.iconsVerticalSpacing
                }
                name: Icons.MailReplyIcon
                color: UbuntuColors.ash
                AbstractButton {
                    anchors.fill: parent
                    onClicked: dekko.showNotice("Not implemented yet. Fix it before release!!!!")
                }
            }
        }
    }

    Column {
        id: cleanLayout
        visible: gs.isCleanLayout
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: units.gu(0.5)

        Label {
            anchors {
                left: parent.left
                right: parent.right
            }
            text: msg.prettyLongDate
            horizontalAlignment: Text.AlignHCenter
            fontSize: "small"
        }
        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: s.height
            Label {
                id: s
                width: parent.width - units.gu(6)
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                text: msg.subject
                horizontalAlignment: Text.AlignHCenter
                fontSize: "medium"
                font.weight: Font.DemiBold
            }
        }
        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: r.height
            Item {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                width: r.width + d.width + div.width + units.gu(2)
                Label {
                    id: r
                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    color: UbuntuColors.blue
                    text: msg.from.name
                    fontSize: "small"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: PopupUtils.open("qrc:/qml/popovers/RecipientPopover.qml", r, {address: msg.from})
                    }
                }
                Label {
                    id: div
                    text: "|"
                    anchors {
                        left: r.right
                        leftMargin: units.gu(1)
                        top: parent.top
                    }
                    fontSize: "small"
                }
                Label {
                    id: d
                    anchors {
                        top: parent.top
                        left: div.right
                        leftMargin: units.gu(1)
                    }
                    color: UbuntuColors.blue
                    text: qsTr("Details")
                    fontSize: "small"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: PopupUtils.open("qrc:/qml/popovers/MessageDetailsPopover.qml", d, {message: msg})
                    }
                }
            }
        }
    }
}

