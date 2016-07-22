import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Contacts 1.0
import "../components"
import "../../actions/contacts"
import "../../actions/views"
import "../../stores/contacts"
import "../../constants"


DekkoPage {
    pageHeader.title: "Addressbooks"
    pageHeader.backAction: Action {
        iconName: "back"
        onTriggered: {
            ViewActions.popStage()
        }
    }

    PageContent {
        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
                model: ContactsStore.abooks.model
                add: DekkoAnimation.listViewAddTransition
                addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
                remove: DekkoAnimation.listViewRemoveTransition
                removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition
                currentIndex: ContactsStore.selectedAddressBookIndex
                highlight: Style.highlightBar
                highlightFollowsCurrentItem: true
                highlightMoveDuration: 200
                delegate: ListItem {
                    height: layout.height
                    ListItemLayout {
                        id: layout
                        title.text: model.name

                        Icon {
                            height: units.gu(3)
                            width: height
                            name: "contact-group"
                            SlotsLayout.position: SlotsLayout.Leading
                        }
                    }
                    onClicked: ContactActions.setSelectedAddressBook(model.index)
                }
            }
        }
    }
}
