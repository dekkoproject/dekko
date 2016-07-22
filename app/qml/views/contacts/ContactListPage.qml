import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Contacts 1.0
import "../components"
import "../../constants"
import "../../actions/contacts"
import "../../actions/views"
import "../../stores/contacts"


DekkoPage {
    id: contactsListPage
    pageHeader.title: addressBook ? addressBook.name : qsTr("Address book")
    pageHeader.backAction:!dekko.viewState.isLargeFF ? bkAction : null

    Action {
        id: bkAction
        iconName: "back"
        onTriggered: ViewActions.popStage()
    }

    property AddressBook addressBook: ContactsStore.selectedAddressBook

    PageContent {
        ContactsListView {
            anchors.fill: parent
            addressBook: contactsListPage.addressBook
        }
    }
}

