import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../components"

ListItem {
    id: root
    height: ctxtLayout.height
    divider.visible: false
    color: UbuntuColors.porcelain
    property string description
    property alias actionIcon: leadingImage.name
    property alias actionIconColor: leadingImage.color
    property alias selectedIcon: trailingImage.name
    property alias selectedIconColor: trailingImage.color
    property alias selected: trailingImage.visible

    // So aparently trying to show a highlight on mouse over is
    // a partcularly annoying thing! Just binding to MouseArea::containsMouse
    // results in all delegates being highlighted on the context menu's completion
    // So instead we have to late bind containsMouse using a short timer binding
    // containsMouse to the highlight rectangle.
    Rectangle {
        id: h
        property bool canShow: false
        anchors.fill: parent
        visible: false
        color: UbuntuColors.silk
    }
    MouseArea {
        id: ma
        anchors.fill: h
        onClicked: root.clicked()
    }
    Binding {
        target: h
        property: "visible"
        value: (ma.containsMouse || ma.pressed)
        when: h.canShow
    }
    Timer {
        id: bindTimer
        interval: 250
        repeat: false
        onTriggered: {
            h.canShow = true
            ma.hoverEnabled = true
        }
    }

    ListItemLayout {
        id: ctxtLayout
        height: units.gu(5)
        title.text: description
        CachedImage {
            id: leadingImage
            height: units.gu(3); width: height
            SlotsLayout.position: SlotsLayout.Leading
        }
        CachedImage {
            id: trailingImage
            height: units.gu(3); width: height
        }
    }



    Component.onCompleted: bindTimer.start()
}
