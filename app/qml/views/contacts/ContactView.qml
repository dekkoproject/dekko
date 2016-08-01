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
    pageHeader.title: qsTr("Contact")
    pageHeader.backAction:!dekko.viewState.isLargeFF ? bkAction : null

    property Contact contact

    Action {
        id: bkAction
        iconName: "back"
//        onTriggered: ViewActions.popStage()
    }

    PageFlickable {
        margins: 0
        ListItem {
            height: layout1.height
            divider.visible: false
            ListItemLayout {
                id: layout1
                title.text: contact.firstname
                Avatar {
                    height: units.gu(5.5)
                    width: height
                    fontSize: "large"
                    name: contact.firstname
                    initials: contact.initials
                    email: contact.emailAddresses.count ? contact.emailAddresses.getFirst().address : ""
                    SlotsLayout.position: SlotsLayout.Leading
                }
            }
        }

        SectionHeader {
            text: qsTr("Email")
            visible: contact.emailAddresses.count
            dividerVisible: false
        }

        Repeater {
            model: contact.emailAddresses
            ListItem {
                height: em.height
                ListItemLayout {
                    id: em
                    title.text: model.address
                }
            }
        }

        SectionHeader {
            text: qsTr("Phone")
            visible: contact.contactNumbers.count
            dividerVisible: false
        }

        Repeater {
            model: contact.contactNumbers
            ListItem {
                height: layout2.height
                ListItemLayout {
                    id: layout2
                    title.text: model.number
                }
            }
        }

        SectionHeader {
            text: qsTr("Address")
            dividerVisible: false
        }
        ListItem {
            height: layout3.height
            divider.visible: false
            ListItemLayout {
                id: layout3
                title.text: contact.address.street
            }
        }
        ContactDetail {
            type: qsTr("Street")
            value: contact.address.street
        }
        ContactDetail {
            type: qsTr("City")
            value: contact.address.city
        }
        ContactDetail {
            type: qsTr("Zip")
            value: contact.address.zip
        }
        ContactDetail {
            type: qsTr("Country")
            value: contact.address.country
        }
    }
}
