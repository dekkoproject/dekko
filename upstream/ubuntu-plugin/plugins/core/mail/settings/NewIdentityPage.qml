import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import Dekko.Ubuntu.Components 1.0

SettingsGroupPage {
    id: root
    property string title: ""
    pageHeader.title: title
    pageHeader.primaryAction: Action {
        iconName: "save"
        onTriggered: {
            SettingsActions.saveIdentity()
            ViewActions.popStageArea(stageArea.stageID)
        }
    }

    IdentityInput {
        newIdentity: true
    }
}
