import QtQuick 2.4
import Dekko.Components 1.0

PixelPerfectItem {
    property alias color: helper.color
    property alias name: helper.icon
    // set's the images horizontal & vertical ratio in terms of width/height of the component
    // Default = 1.0
    property alias horizontalRatio: helper.horizontalRatio
    property alias verticalRatio: helper.verticalRatio
    Image {
        id: root
        anchors.fill: parent
        cache: true;
        smooth: false;
        fillMode: Image.Pad;
        asynchronous: true;
        antialiasing: false;
        ImageHelper on source {
            id: helper
            size: root.height
        }
    }
}
