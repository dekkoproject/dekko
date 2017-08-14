import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
import QuickFlux 1.0
import PlugMan 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0

Item {
    id: composer
    anchors.fill: parent

    StretchColumn {
        anchors.fill: parent

        Rectangle {
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }
            height: units.gu(6)
            implicitHeight: height
            color: UbuntuColors.porcelain
            StretchRow {
                id: row
                anchors.fill: parent
                anchors.leftMargin: units.gu(1)
                anchors.rightMargin: units.gu(1)
                spacing: units.gu(1)
                readonly property bool textVisible: composer.width > units.gu(60)
                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: ComposerStore.actions.discardMessageAction
                    showLabel: row.textVisible
                }
                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: ComposerStore.actions.saveDraftAction
                    showLabel: row.textVisible
                }

                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: Action {
                        text: qsTr("Attach")
                        iconName: Icons.AttachmentIcon
                        iconSource: Paths.actionIconUrl(Icons.AttachmentIcon)
                        onTriggered: ContentActions.pickFile(composer)
                    }
                    showLabel: row.textVisible
                }

                Stretcher {}

                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: ComposerStore.actions.sendAction
                    showLabel: row.textVisible
                }
            }

            Line {
                anchors {
                    bottom: parent.bottom
                    right: parent.right
                    left: parent.left
                }
            }
        }
//        Rectangle {
//            id: toolbar
//            anchors {
//                left: parent.left
//                right: parent.right
//            }
//            height: composePanel.textFieldFocused ? units.gu(5) : 0
//            implicitHeight: height
//            color: UbuntuColors.porcelain
//            Line {
//                anchors {
//                    bottom: parent.bottom
//                    right: parent.right
//                    left: parent.left
//                }
//            }
//            Behavior on height { UbuntuNumberAnimation {} }
//        }

        Stretcher {
            id: content

            StretchRow {
                anchors.fill: parent
                spacing: 0

                Stretcher {
                    implicitHeight: parent.height
                    MessageComposer {
                        id: composePanel
                        anchors.fill: parent
                    }
                }
                ComposeSidePanel {}
            }
        }

    }
}
