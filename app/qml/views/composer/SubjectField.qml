import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Themes.Ambiance 1.3
import Dekko.Components 1.0
import "../components"
import "../../constants"

FocusScope {

    readonly property int defaultHeight: units.gu(4)
    readonly property alias text: input.text
    property alias textDocument: input.textDocument

    height: input.lineCount > 1 ? input.height + Style.smallSpacing : defaultHeight
    anchors {
        left: parent.left
        right: parent.right
    }
    implicitHeight: height
    implicitWidth: width
    clip: true

    StretchRow {
        anchors {
            fill: parent
            leftMargin: Style.defaultSpacing
            rightMargin: Style.defaultSpacing
        }
        spacing: Style.smallSpacing
        Label {
            anchors {
                left: parent.left
                top: parent.top
                topMargin: Style.smallSpacing
            }
            text: qsTr("Subject:")
        }
        Stretcher {
            height: input.height
            implicitHeight: height

            TextArea {
                id: input
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                    topMargin: Style.smallSpacing
                }
                maximumLineCount: 3
                wrapMode: TextEdit.WrapAnywhere
                autoSize: true
                height: defaultHeight
                style:TextAreaStyle {
                    overlaySpacing: 0
                    frameSpacing: 0
                    background: Item {}
                }
            }
        }
    }

    Line {
        anchors {
            bottom: parent.bottom
            right: parent.right
            left: parent.left
        }
    }
}

