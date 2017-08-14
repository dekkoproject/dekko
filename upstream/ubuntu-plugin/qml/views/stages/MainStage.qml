import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0
import Dekko.Ubuntu.Stage 1.0
import Dekko.Components 1.0
import PlugMan 1.0
import Dekko.Ubuntu.Stage 1.0
import Dekko.Ubuntu.Components 1.0
import "../"

BaseStage {
    id: ms

    panelEnabled: devModeEnabled

    // We use a stretch row here rather than RowLayout
    // Just because we can use the implicit size hints to stretch the correct
    // panel. Yes we could use Layout.fillWidth but in the future there maybe
    // more columns added to this (through plugins) and we may want to share remaining width evenly between two
    // or more colums. Which StretchRow handles nicely
    StretchRow {
        spacing: 0
        anchors.fill: parent

        NavPluginSideBar {
            height: parent.height
            width: units.gu(8)
            implicitWidth: width
            visible: !dekko.isSmallFF && !AccountSetup.wizardRunning
        }

        Stretcher {
            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            StageStack {
                id: rootPageStack
                anchors.fill: parent

                Component.onCompleted: {
                    ViewActions.stageStackReady()
                }
            }
        }
    }
}

