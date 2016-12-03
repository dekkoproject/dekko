import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as AreTheseStillValid
import Dekko.Mail.Accounts 1.0

Item {

    property var selectedMethod: encryptionMethodModel.count ? encryptionMethodModel.get(selectedIndex).method : AccountConfig.None
    property alias selectedIndex: cryptSelector.selectedIndex
    height: col.height
    anchors {
        left: parent.left
        right: parent.right
    }

    function setEncryptionType(type) {
        encryptionMethodModel.loadCryptModel()
        var idx = 0;
        switch(type) {
        case AccountConfig.None:
            break
        case AccountConfig.STARTTLS:
            idx = 1
            break
        case AccountConfig.SSL:
            idx = 2
            break
        }
        cryptSelector.selectedIndex = idx
    }

    Column {
        id: col
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }
        spacing: units.gu(1)

        Label {
            anchors {
                left: parent.left
                right: parent.right
            }
            text: qsTr("Encryption")
        }
        AreTheseStillValid.ItemSelector {
            id: cryptSelector
            objectName: "encryptionSelector"

            model: encryptionMethodModel
            delegate: OptionSelectorDelegate {
                text: model.description
            }

            ListModel {
                id: encryptionMethodModel

                function loadCryptModel() {
                    encryptionMethodModel.append({ "description": qsTr("No encryption"), "method": AccountConfig.None })
                    encryptionMethodModel.append({ "description": qsTr("Use encryption (STARTTLS)"), "method": AccountConfig.STARTTLS })
                    encryptionMethodModel.append({ "description": qsTr("Force encryption (SSL/TLS)"), "method": AccountConfig.SSL })
                }
            }
//            Component.onCompleted: encryptionMethodModel.loadCryptModel()
        }
    }
}


