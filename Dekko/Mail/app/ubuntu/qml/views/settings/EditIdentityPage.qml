import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import "../components"

SettingsGroupPage {
    id: root
    property string title: ""
    pageHeader.title: title
    pageHeader.primaryAction: Action {
        iconName: "save"
        onTriggered: SettingsActions.saveIdentity()
    }

    IdentityInput {
        newIdentity: false
    }
}
