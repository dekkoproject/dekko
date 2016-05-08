pragma Singleton
import QtQuick 2.4
import Ubuntu.Components 1.3

QtObject {

    // The defaults spacing is 2 gu around text, between icons etc... (That's a design rule btw!)
    readonly property int defaultSpacing: units.gu(2)
    readonly property int largeSpacing: (defaultSpacing * 2)
    readonly property int smallSpacing: (defaultSpacing / 2)
    // Used by line component.
    readonly property int lineSize: units.dp(1)
    readonly property color lineColor: UbuntuColors.lightGrey

    readonly property int defaultIconSize: units.gu(2.5)
    readonly property int largeIconSize: units.gu(3.5)
    readonly property int smallIconSize: units.gu(1.5)

    readonly property int defaultPanelWidth: units.gu(35)

    property Rectangle highlightBar: Rectangle {
        color: Qt.rgba(0, 0, 0, 0.05)
    }
}

