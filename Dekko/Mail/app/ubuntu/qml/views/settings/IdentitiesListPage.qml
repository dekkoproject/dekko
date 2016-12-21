import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import "../components"

DekkoPage {

    pageHeader {
        title: qsTr("Identities")
        backAction: Action {
            iconName: "back"
            onTriggered: {
                ViewActions.popStageArea(ViewKeys.settingsStack2)
                ViewActions.popStageArea(ViewKeys.settingsStack1)
            }
        }
        primaryAction: Action {
            iconName: "contact-new"
            onTriggered: SettingsActions.openSettingsGroup(Qt.resolvedUrl("./NewIdentityPage.qml"))
        }
    }

    PageContent {

        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
            }
        }
    }
}
