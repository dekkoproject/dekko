import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Markdown 1.0

ScrollView {
    id: editor
    property int frameSpacing: 0
    property alias headerHeight: col.height

    property alias header: h.data
    property alias toolbar: t.data

    anchors.fill: parent
    Flickable {
        id: flicker
        anchors.fill: parent
        contentHeight: col.height + te.paintedHeight + editor.frameSpacing + units.gu(1)
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        Column {
            id: col
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            Item {
                id: h
                height: childrenRect.height
                width: parent.width
            }
            Item {
                id: t
                height: childrenRect.height
                width: parent.width
            }
        }

        TextEdit {
            id: te
            anchors {
                left: parent.left
                top: col.bottom
                right: parent.right
                margins: editor.frameSpacing
            }
            height: Math.max(editor.height - col.height, contentHeight)
            font.family: "Ubuntu"
            font.pixelSize: FontUtils.sizeToPixels("medium")
            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
            mouseSelectionMode: TextEdit.SelectWords
            persistentSelection: true
            selectByMouse: true
            selectionColor: Qt.rgba(UbuntuColors.blue.r, UbuntuColors.blue.g, UbuntuColors.blue.b, 0.2)
            selectedTextColor: UbuntuColors.jet
            Keys.forwardTo: [inputOverlay]
            Mouse.forwardTo: [inputOverlay]
            InputOverlay {
                id: inputOverlay
                anchors.fill: parent
                root: editor
                textEdit: te
                flickable: flicker
            }
            MarkdownDocument {
                document: te.textDocument
            }
        }
    }
}
