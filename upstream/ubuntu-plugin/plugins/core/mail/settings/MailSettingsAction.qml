import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0

Action {
    iconName: "mail-unread"
    text: qsTr("Mail")
    onTriggered: ViewActions.pushToStageArea(
                    ViewKeys.settingsStack1,
                    Qt.resolvedUrl("./MailSettings.qml"),
                    {}
                 )
}
