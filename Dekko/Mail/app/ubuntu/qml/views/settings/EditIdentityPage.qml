import QtQuick 2.4
import Ubuntu.Components 1.3
import "../components"

SettingsGroupPage {
    id: root
    property string title: ""
    pageHeader.title: title

    IdentityInput {
        newIdentity: false
    }
}
