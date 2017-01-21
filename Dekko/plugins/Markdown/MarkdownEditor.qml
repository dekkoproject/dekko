import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Markdown 1.0

ScrollView {
    id: editor
    property int frameSpacing: 0
    property bool plainTextOnly: false
    property alias headerHeight: col.height
    property alias options: mdDoc.options
    property alias text: te.text
    property alias textDocument: te.textDocument

    property alias header: h.data
    property alias toolbar: t.data

    signal showPreview()

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
            Item {
                width: parent.width
                height: btn.height
                visible: te.text && !editor.plainTextOnly
                AbstractButton {
                    id: btn
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: units.gu(10)
                    height: l.height + units.gu(1)

                    Label {
                        id: l
                        anchors {
                            right: parent.right
                            rightMargin: units.gu(3)
                            verticalCenter: parent.verticalCenter
                        }
                        text: qsTr("Preview")
                        color: UbuntuColors.blue
                    }
                    onClicked: showPreview()
                }
            }
        }

        TextEdit {
            id: te
            anchors {
                left: parent.left
                top: col.bottom
                right: parent.right
                leftMargin: editor.frameSpacing
                topMargin: text && !editor.plainTextOnly ? 0 : editor.frameSpacing
                rightMargin: editor.frameSpacing
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
            Keys.forwardTo: editor.plainTextOnly ? [inputOverlay] : [mdDoc, inputOverlay]
            Mouse.forwardTo: [inputOverlay]

            InputOverlay {
                id: inputOverlay
                anchors.fill: parent
                root: editor
                textEdit: te
                flickable: flicker
            }

            Binding {
                target: mdDoc
                property: "textDocument"
                value: te.textDocument
                when: !editor.plainTextOnly
            }

            MarkdownDocument {
                id: mdDoc
                options: FormattingOptions {
                    autoMatchEnabled: true
                    cycleBulletMarker: true
                    spacesForTabs: true
                    tabWidth: 4
                    textColor: UbuntuColors.slate
                    backgroundColor: "white"
                    linkColor: UbuntuColors.blue
                    markupColor: UbuntuColors.graphite
                }

                cursorPosition: te.cursorPosition
                onCursorPositionChanged: {
                    if (te.cursorPosition !== cursorPosition) {
                        te.cursorPosition = cursorPosition
                    }
                }
                hasSelection: te.selectedText
                selectionStart: te.selectionStart
                selectionEnd: te.selectionEnd
            }
        }
    }
}
