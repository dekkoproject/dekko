import QtQuick 2.4
import QtQuick.Window 2.1
import Ubuntu.Components 1.3
import MazDB 1.0
import "./views/components"
import Dekko.Markdown 1.0
import QuickFlux 1.0
import Dekko.Mail.API 1.0

Window {
    id: window
    visible: true
    minimumWidth: units.gu(30)
    minimumHeight: units.gu(30)
    width: units.gu(120)
    height: units.gu(80)
    flags: Qt.Window
    modality: Qt.NonModal

    title: qsTr("Dekko Mail")

    MazDBSettings {
        category: "window-property-cache"
        property alias windowWidth: window.width
        property alias windowHeight: window.height
        property alias windowVisible: window.visible
        property alias windowX: window.x
        property alias windowY: window.y
    }

    Item {
        anchors.fill: parent
        Loader {
            asynchronous: false
            anchors.fill: parent
            source: Qt.resolvedUrl("./Dekko.qml")
        }
    }
//        MarkdownEditor {
//            anchors.fill: parent
//            frameSpacing: units.gu(2)
//            header: Rectangle {
//                width: parent.width
//                height: units.gu(10)
//                color: UbuntuColors.orange
//            }
//            toolbar: Rectangle {
//                width: parent.width
//                height: units.gu(7)
//                color: UbuntuColors.green
//            }
//            onShowPreview: {
//                if (d.htmlViewerWindow !== null) {
//                    d.htmlViewerWindow.destroy()
//                }
//                var w = htmlWin.createObject(root, {html: content})
//                w.show()
//                d.htmlViewerWindow = w
//            }
//        }
//    }

//    Component {
//        id: htmlWin
//        Window {
//            id: htmlWindow
//            width: units.gu(40)
//            height: units.gu(70)
//            property string html: ""
//            HtmlViewer {
//                html: htmlWindow.html
//                anchors.fill: htmlWindow.contentItem
//            }
//        }
//    }

//    QtObject {
//        id: d
//        property Window htmlViewerWindow: null
//    }

////    Filter {
////        type: ContentKeys.displayHtmlContent
////        onDispatched: {
////            if (d.htmlViewerWindow !== null) {
////                d.htmlViewerWindow.destroy()
////            }
////            var w = htmlWin.createObject(dekko, {html: message.content})
////            w.show()
////            d.htmlViewerWindow = w
////        }
////    }
}
