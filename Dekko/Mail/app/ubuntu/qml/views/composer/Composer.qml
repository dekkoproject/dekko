import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0
import QuickFlux 1.0
import PlugMan 1.0
import "../components"
import "../../constants"

Item {
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
                anchors.fill: parent
                anchors.leftMargin: units.gu(1)
                anchors.rightMargin: units.gu(1)
                spacing: units.gu(1)
                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: ComposerStore.actions.discardMessageAction
                }
                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: ComposerStore.actions.saveDraftAction
                }

                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: ComposerStore.actions.attachmentsAction
                }

                Stretcher {}

                TitledHeaderButton {
                    height: units.gu(5)
                    anchors.verticalCenter: parent.verticalCenter
                    action: ComposerStore.actions.sendAction
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
        Stretcher {
            id: content

            StretchRow {
                anchors.fill: parent
                spacing: 0

                Stretcher {
                    implicitHeight: parent.height
                    MessageComposer {
                        anchors.fill: parent
                    }
                }
                ComposeSidePanel {}
            }
        }

    }
}
