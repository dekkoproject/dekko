import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../utils/UiUtils.js" as UiUtils
import "../../constants"

PixelPerfectItem {
    id: avatar
    property string name
    property string initials
    property string email
    property string radius: "small"
    property bool validContact: true
    property string fontSize: "large"

    height: units.gu(6)
    width: height
    Component {
        id: component_inner_av
        UbuntuShape {
            id: avatarCircle
            anchors.fill: parent
            anchors.centerIn: parent
            aspect: UbuntuShape.Flat
            radius: avatar.radius
            backgroundColor: UiUtils.getIconColor(name)
            visible: validContact
            //name: "avatar-circle"
            Label {
                id: initialsLabel
                color: "#FFFFFF"
                text: validContact ? initials : ""
                anchors.centerIn: parent
                fontSize: avatar.fontSize
            }

            Icon {
                id: contactIcon
                name: "contact"
                anchors.centerIn: parent
                height: parent.height - Style.defaultSpacing
                width: height
                color: "white"
                visible: !initialsLabel.text
            }
        }
    }
    Loader {
        id: loader_inner_av
        anchors.fill: parent
        active: !inner_name.visible
        sourceComponent: component_inner_av
    }
    UbuntuShapeGravatar {
        id: inner_name
        radius: avatar.radius
        emailAddress: validContact /*&& GlobalSettings.preferences.gravatarAllowed */? email : ""
        clip: true
        anchors.fill: parent
        visible: status === Image.Ready
    }
}

