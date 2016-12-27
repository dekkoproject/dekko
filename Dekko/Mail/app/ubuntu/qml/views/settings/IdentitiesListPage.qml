import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0
import Dekko.Mail.Stores.Settings 1.0
import MazDB 1.0
import "../../constants"
import "../components"
import "../models"

DekkoPage {

    pageHeader {
        title: qsTr("Identities")
        backAction: Action {
            iconName: "back"
            onTriggered: {
                ViewActions.popStageArea(ViewKeys.settingsStack2)
                ViewActions.popStageArea(ViewKeys.settingsStack1)
            }
        }
        primaryAction: Action {
            iconName: "contact-new"
            onTriggered: SettingsActions.openSettingsGroup(Qt.resolvedUrl("./NewIdentityPage.qml"))
        }
    }

    MazDBSortProxy {
        id: proxy
        sortCallBack: function (left, right) {
            return left < right
        }
        // Sort by the accountName as we use this for the sections
        sortBy: "accountName"
        // make sure the listmodel is initialized before adding it to the proxy
        model: identitiesModel.source ? identitiesModel : null

    }

    IdentitiesListModel {
        id: identitiesModel
    }

    PageContent {

        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
                section.property: "accountName"
                section.delegate: Item {
                    height: units.gu(4)
                    Label {
                        anchors {
                            left: parent.left
                            leftMargin: units.gu(1)
                            verticalCenter: parent.verticalCenter
                        }
                        text: section
                    }
                }

                add: DekkoAnimation.listViewAddTransition
                addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
                remove: DekkoAnimation.listViewRemoveTransition
                removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition
                model: proxy.model ? proxy : null
                delegate:  ListItem {
                    height: layout.height
                    leadingActions: ListItemActions {
                        actions: [
                            Action {
                                iconName: "delete"
                                onTriggered: AccountActions.removeIdentity(model.id)
                            }
                        ]
                    }

                    onClicked: {
                        // If there is already an identity open for editing
                        // we want to prevent it saving overwise it will overwrite
                        // the tempIdentity we store here on page destruction
                        // as a result of calling openSettingsGroup
                        SettingsStore.saveGroupDisabled = true
                        // Indicate this is an already existing identity
                        SettingsStore.tempIsNew = false
                        // Set the values to load
                        SettingsStore.tempIdentity = model.value
                        SettingsActions.openSettingsGroup(Qt.resolvedUrl("./EditIdentityPage.qml"))
                        // re-enable saving settings groups so that we save state on layout transitions
                        SettingsStore.saveGroupDisabled = false
                    }

                    ListItemLayout {
                        id: layout
                        title.text: model.name
                        subtitle.text: model.email
                        summary.text: model.signature
                        summary.maximumLineCount: 1
                    }
                }
            }
        }
    }
}
