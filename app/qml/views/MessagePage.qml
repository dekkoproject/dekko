import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail 1.0
import "../components"
import "../webview"

DekkoPage {

    pageHeader.title: "Message"
    pageHeader.backAction: Action {
        id: back
        iconName: "back"
        onTriggered: internalStack.pop()
    }

    property alias msgId: message.messageId
    Message {
        id: message
        onBodyChanged: {
            console.log("MainPartUrl: ", body)
            webview.setCidQuery(message.messageId)
            webview.setBodyUrl(body)
        }
    }

    Item {
        anchors {
            left: parent.left
            right: parent.right
            top: pageHeader.bottom
            bottom: parent.bottom
        }
        DekkoWebView {
            id: webview
            anchors.fill: parent
            url: "about:blank"
        }
    }
}

