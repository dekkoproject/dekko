import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Contacts 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0
import "../../actions/views"


DekkoPage {
    id: contactsListPage
    pageHeader.title: qsTr("Contact")
    pageHeader.backAction:!dekko.isLargeFF ? bkAction : null

    property Contact contact
    property bool editModeEnabled: false

    pageHeader.primaryAction: editModeEnabled ? saveAction : editAction

    Action {
        id: editAction
        iconName: "edit"
        onTriggered: editModeEnabled = true
    }

    Action {
        id: saveAction
        iconName: "save"
        onTriggered: {
            ContactActions.saveContact(contact)
            editModeEnabled = false
        }
    }

    Action {
        id: bkAction
        iconName: "back"
//        onTriggered: ViewActions.popStage()
    }

    PageFlickable {
        margins: 0
//        spacing: 0
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
        ContactGroup {
            title: qsTr("Email")
            visible: contact.emailAddresses.count
            spacing: editModeEnabled ? units.gu(1) : 0
            Repeater {
                model: contact.emailAddresses
                ContactDetail {
                    detail: model.address
                    editable: editModeEnabled
                    detailAction: Action {
                        iconName: "ok"
                        onTriggered: console.log("ACTION CLICKED")
                    }
                    onValueChanged: model.qtObject.address = val
                }
            }
        }
        ContactGroup {
            title: qsTr("Phone")
            visible: contact.contactNumbers.count
            spacing: units.gu(1)
            Repeater {
                model: contact.contactNumbers
                ContactDetail {
                    title: model.qtObject.typeToString(model.type)
                    detail: model.number
                    editable: editModeEnabled
                    onValueChanged: model.qtObject.number = val
                }
            }
        }
        ContactGroup {
            title: qsTr("Address")
            spacing: units.gu(1)
            ContactDetail {
                editable: editModeEnabled
                title: qsTr("Street")
                detail: contact.address.street
                onValueChanged: contact.address.street = val
            }
            ContactDetail {
                editable: editModeEnabled
                title: qsTr("City")
                detail: contact.address.city
                onValueChanged: contact.address.city = val
            }
            ContactDetail {
                editable: editModeEnabled
                title: qsTr("Zip")
                detail: contact.address.zip
                onValueChanged: contact.address.zip = val
            }
            ContactDetail {
                editable: editModeEnabled
                title: qsTr("Country")
                detail: contact.address.country
                onValueChanged: contact.address.country = val
            }
        }
    }
}
