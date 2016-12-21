import QtQuick 2.4
import Ubuntu.Components 1.3
import "../components"

PageFlickable {
    id: input
    property bool newIdentity: false


    TitledTextField {
        id: name
        title: qsTr("Name")
        text: account.outgoing.name
    }

    TitledTextField {
        id: email
        title: qsTr("Email Address")
        text: account.outgoing.email
    }

    TitledTextField {
        id: replyTo
        title: qsTr("Reply-To")
        text: ""
    }

    Label {
        text: qsTr("Signature")
    }

    TextArea {
        anchors {
            left: parent.left
            right: parent.right
        }
    }

    states: [
        State {
            name: "new"
            when: input.newIdentity
            PropertyChanges {
                target: root
                title: qsTr("New identity")
            }
        },
        State {
            name: "edit"
            when: !input.newIdentity
            PropertyChanges {
                target: root
                title: qsTr("Edit identity")
            }
        }
    ]
}
