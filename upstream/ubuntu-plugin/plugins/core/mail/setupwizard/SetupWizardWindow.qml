import QtQuick 2.4
import QtQuick.Window 2.1
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import MazDB 1.0
import Dekko.Controls 1.0
import Dekko.Ubuntu.Constants 1.0
import Dekko.Ubuntu.Components 1.0

Window {
    id: window
    visible: true
    minimumWidth: units.gu(30)
    minimumHeight: units.gu(30)
    width: units.gu(60)
    height: units.gu(60)
    flags: Qt.Window
    modality: Qt.ApplicationModal

    title: qsTr("Mail Setup Wizard")

    onClosing: {
        ViewActions.closeSetupWizard()
    }

    MazDBSettings {
        category: "setupwizard-property-cache"
        property alias windowWidth: window.width
        property alias windowHeight: window.height
    }

    Item {
        anchors.fill: parent
        SetupWizard {
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
                bottom: kbdRect.top
            }
        }

        KeyboardRectangle {
            id: kbdRect
            anchors {
                left: parent.left
                bottom: parent.bottom
                right: parent.right
            }
        }

//        Component.onCompleted: AccountSetup.wizardRunning = true
//        Component.onDestruction: AccountSetup.wizardRunning = false
    }
}
