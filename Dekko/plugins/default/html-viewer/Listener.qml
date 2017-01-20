import QtQuick 2.4
import QtQuick.Window 2.1
import QuickFlux 1.0
import Dekko.Mail.API 1.0

AppListener {

    Component {
        id: htmlWin
        Window {
            id: htmlWindow
            width: units.gu(70)
            height: units.gu(70)
            modality: Qt.WindowModal
            property string html: ""
            HtmlViewer {
                html: htmlWindow.html
                anchors.fill: htmlWindow.contentItem
            }
        }
    }

    QtObject {
        id: d
        property Window htmlViewerWindow: null
    }

    Filter {
        type: ContentKeys.displayHtmlContent
        onDispatched: {
            if (d.htmlViewerWindow !== null) {
                d.htmlViewerWindow.destroy()
            }
            var w = htmlWin.createObject(message.caller, {html: message.content})
            w.show()
            d.htmlViewerWindow = w
        }
    }
}
