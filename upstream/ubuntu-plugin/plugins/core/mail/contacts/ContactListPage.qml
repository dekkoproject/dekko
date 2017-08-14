import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Mail.API 1.0
import Dekko.Contacts 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0


DekkoPage {
    id: contactsListPage
    pageHeader.title: addressBook ? addressBook.name : qsTr("Address book")
    pageHeader.backAction:!dekko.isLargeFF ? bkAction : null

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
            onContactClicked: ViewActions.replaceTopStageAreaItem(
                                  ContactKeys.contactViewStageArea,
                                  Qt.resolvedUrl("./ContactView.qml"),
                                  {contact: contact})
        }
    }
}

