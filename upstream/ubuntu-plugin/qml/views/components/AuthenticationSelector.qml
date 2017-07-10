import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as AreTheseStillValid
import Dekko.Mail.Accounts 1.0

Item {

    property var selectedMethod: authMethodModel.count ? authMethodModel.get(selectedIndex).method : AccountConfig.PLAIN
    property alias selectedIndex: authSelector.selectedIndex
    height: col.height
    anchors {
        left: parent.left
        right: parent.right
    }

    function setAuthType(type) {
        authMethodModel.loadCryptModel()
        var idx = 0;
        switch(type) {
        case AccountConfig.PLAIN:
            break
        case AccountConfig.LOGIN:
            idx = 1
            break
        case AccountConfig.CRAM_MD5:
            idx = 2
            break
        }
        authSelector.selectedIndex = idx
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
            text: qsTr("Authentication")
        }
        AreTheseStillValid.ItemSelector {
            id: authSelector
            objectName: "encryptionSelector"

            model: authMethodModel
            delegate: OptionSelectorDelegate {
                text: model.description
            }

            ListModel {
                id: authMethodModel

                function loadCryptModel() {
                    authMethodModel.append({ "description": qsTr("PLAIN"), "method": AccountConfig.PLAIN })
                    authMethodModel.append({ "description": qsTr("LOGIN"), "method": AccountConfig.LOGIN })
                    authMethodModel.append({ "description": qsTr("CRAM-MD5"), "method": AccountConfig.CRAM_MD5 })
                }
            }
//            Component.onCompleted: authMethodModel.loadCryptModel()
        }
    }
}
