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
    readonly property color selectedHighlight: UbuntuColors.blue
    readonly property color windowColor: "#FFFFFF"
    property color colorDumb : "#FF00FF"; // magenta

    readonly property int defaultIconSize: units.gu(2.5)
    readonly property int largeIconSize: units.gu(3.5)
    readonly property int smallIconSize: units.gu(1.5)

    readonly property int defaultPanelWidth: units.gu(35)

    readonly property color devPanelBackground: "#212121"

    property Component highlightBar: Component {
        Rectangle {
            color: Qt.rgba(0, 0, 0, 0.05)
        }
    }

    property Item _garbage_ : Item { }
    property Component templateGradientShaded : Component {
        Gradient {
            id: autogradient;

            property color baseColorTop    : colorDumb;
            property color baseColorBottom : colorDumb;

            GradientStop { color: autogradient.baseColorTop;    position: 0.0; }
            GradientStop { color: autogradient.baseColorBottom; position: 1.0; }
        }
    }
    function gradientShaded (baseColorTop, baseColorBottom) {
        return templateGradientShaded.createObject (_garbage_, {
                                                        "baseColorTop"    : (baseColorTop    || selectedHighlight),
                                                        "baseColorBottom" : (baseColorBottom || windowColor),
                                                    });
    }
}

