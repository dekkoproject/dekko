import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3 as ListItems
import Dekko.Contacts 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0
import "../../actions/views"

Item {

    readonly property bool slimMode: state === ContactKeys.slimPicker
    readonly property bool standardMode: state === ContactKeys.standardPicker
    // if no addressbook is set then the combined contacts model is used.
    // set this if you want to limit results to a single addressbook
    property AddressBook addressBook
    property alias filterEmailOnly: sortedContacts.emailOnly

    signal contactClicked(var contact);

    Item {
        id: sf
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
        }

        height: units.gu(5)
        width: parent.width
        TextField {
            id: tf
            anchors {
                fill: parent
                margins: units.gu(0.5)
            }
            placeholderText: qsTr("Search")
        }
    }

    ScrollView {
        anchors {
            left: parent.left
            top: sf.bottom
            right: parent.right
            bottom: parent.bottom
        }
        ListView {
            id: lv
            anchors.fill: parent
            model: searchModel
//            add: DekkoAnimation.listViewAddTransition
//            addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
//            remove: DekkoAnimation.listViewRemoveTransition
//            removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition

            delegate: ListItem {
                height: layout.height
                property var contact: model.qtObject
                ListItemLayout {
                    id: layout
                    title.text: contact.firstname
                    height: slimMode ? units.gu(5) : units.gu(7)
                    Avatar {
                        height: slimMode ? units.gu(3.5) : units.gu(5.5)
                        width: height
                        fontSize: slimMode ? "medium" : "large"
                        name: contact.firstname
                        initials: contact.initials
                        email: contact.emailAddresses.count ? contact.emailAddresses.getFirst().address : ""
                        SlotsLayout.position: SlotsLayout.Leading
                    }
                }
                onClicked: contactClicked(model.qtObject)
                leadingActions: ListItemActions {
                    actions: [
                        Action {
                            iconName: "delete"
                            onTriggered: ContactActions.removeContact(model.qtObject)
                        }
                    ]
                }
            }

            // Ok so this took a bit of figuring out. We have to add \x00 to
            // be able to use a qtObject property for sections.
            section.property: "section\x00"
            section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart
            section.delegate: Component {
                SectionHeader {
                    text: section
                    backgroundEnabled: true
                }
            }
        }
    }
    state: ContactKeys.standardPicker
    states: [
        State {
            name: ContactKeys.slimPicker
        },
        State {
            name: ContactKeys.standardPicker
        }
    ]
}
