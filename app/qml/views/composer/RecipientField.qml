import QtQuick 2.4
import QuickFlux 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Dekko.Components 1.0
import Dekko.Mail 1.0
import "../../actions/composer"
import "../../actions/logging"
import "../../actions/popups"
import "../../stores/composer"
import "../../constants"
import "../components"
import "../delegates"

FocusScope {
    id: field

    property int recipientType: RecipientType.To
    property alias text: input.text
    property alias textDocument: input.textDocument

    height: main.height

    StretchRow {
        anchors {
            leftMargin: Style.smallSpacing
            fill: parent
            rightMargin: Style.smallSpacing
        }
        spacing: Style.smallSpacing
        // Label to display our To, Cc, Bcc types
        Label {
            id: type
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: Style.smallSpacing
                topMargin: Style.smallSpacing
            }
            text: MailUtils.recipientTypeToString(recipientType) + ":"
        }
        // We want the recipient flow to take up all available space so use a stretcher
        Stretcher {
            id: main
            height: flow.height
            implicitHeight: height
            // Use a flow here to show the RecipientInfo for already added contacts
            // We use the flow to position the RecipientInput by placing it as the last item
            // in the flow and taking up all width. This gives consistent spacing as contacts
            // are added as well as we only have to resize the parent to flows height and not
            // flow height + input height. 1 binding less... yay! heh
            Flow {
                id: flow
                anchors {
                    left: parent.left
                    leftMargin: Style.smallSpacing
                    top: parent.top
                    topMargin: ComposerStore.recipients.to.count ? (Style.smallSpacing / 2) : Style.smallSpacing
                    right: parent.right
                }
                spacing: Style.smallSpacing / 2
                // Display the already added recipients
                Repeater {
                    id: repeater
                    model: ComposerStore.recipients.to
                    delegate: RecipientDelegate {
                        composeMode: true
                        type: recipientType
                    }
                }
                Item {
                    width: parent.width
                    height: units.gu(4)
                    clip: true
                    RecipientInput {
                        id: input
                        anchors {
                            left: parent.left
                            top: parent.top
                            right: parent.right
                        }
                        placeholderText: qsTr("Enter an address")
                        height: parent.height
                        // We don't want to let the return/enter key events
                        // propogate to the textarea as it just moves to a new line
                        // whereas we want it to validate & commit the address to the
                        // recipients model. So we catch and accept the event here.
                        Keys.onReturnPressed: {
                            event.accepted = true
                            console.log("RETURN PRESSED")
                            ComposerActions.addRecipientIfValid(recipientType, input.text)
                        }
                        Keys.onEnterPressed: {
                            event.accepted = true
                            console.log("ENTER PRESSED")
                            ComposerActions.addRecipientIfValid(recipientType, input.text)
                        }
                        // We also support completion tokens. and is triggered
                        // when an address ends with ';' or ','
                        onCompletionTokenSeen: {
                            console.log("COMPLETION TOKEN SEEN")
                            ComposerActions.addRecipientIfValid(recipientType, input.text)
                        }
                    }
                }
            }
        }
        // Finally the input field context actions button
        HeaderButton {
            id: ctxt
            anchors {
                right: parent.right
                top: parent.top
                topMargin: -units.gu(0.5)
            }
            height: units.gu(5)
            width: units.gu(4)
            implicitWidth: width
            implicitHeight: height
            iconSize: units.gu(2.25)
            iconColor: UbuntuColors.ash
            action: Action {
                iconSource: Paths.actionIconUrl(Icons.ContextMenuIcon)
                onTriggered: PopupUtils.open("qrc:/qml/views/composer/RecipientInputContextMenu.qml",
                                             ctxt,
                                             {
                                                 recipientType: field.recipientType
                                             })
            }
        }
    }
    // Divider
    Line {
        anchors {
            left: parent.left
            bottom: parent.bottom
            right: parent.right
        }
    }

    AppScript {
        runWhen: ComposerKeys.addRecipientIfValid
        script: {
            if (message.type === recipientType) {
                ComposerActions.validateAddress(message.type, message.address)
            } else {
                exit.bind(this, 0)
                return;
            }
            once(ComposerKeys.validAddress, function(message) {
                Log.logInfo("RecipientField::validAddress", "Address is valid, adding to recipients")
                ComposerActions.addRecipientFromAddress(message.type, message.address)
                input.text = ""
            })
            // Somethings not right. Show the invalid fields.
            once(ComposerKeys.invalidAddress, function(message) {
                Log.logInfo("RecipientField::invalidAddress", "Address invalid: %1".arg(message.address))
                input.cursorPosition = input.length
                PopupActions.showError("Invalid email address: %1".arg(message.address))
            })
        }
    }
}

