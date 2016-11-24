import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Contacts 1.0
import "../components"
import "../../actions/views"
import "../../constants"


DekkoPage {

    pageHeader {
        title: qsTr("Addressbooks")
        backAction: Action {
            iconName: "back"
            onTriggered: {
                ViewActions.popStage()
            }
        }
    }


    PageContent {
        ScrollView {
            anchors.fill: parent
            ListView {
                anchors.fill: parent
                model: ContactsStore.addressBooksModel
                add: DekkoAnimation.listViewAddTransition
                addDisplaced: DekkoAnimation.listViewAddDisplacedTransition
                remove: DekkoAnimation.listViewRemoveTransition
                removeDisplaced: DekkoAnimation.listViewRemoveDisplacedTransition
                delegate: Item {
                    width: parent.width
                    height: contents.height
                    property var addressbook: model.qtObject
                    Column {
                        id: contents
                        anchors {
                            left: parent.left
                            top: parent.top
                            topMargin: units.gu(1)
                            right: parent.right
                        }

                        Item {
                            width: parent.width
                            height: units.gu(2)
                            Label {
                                anchors {
                                    left: parent.left
                                    leftMargin: units.gu(2)
                                    verticalCenter: parent.verticalCenter
                                }
                                text: addressbook.displayName
                                color: UbuntuColors.slate
                            }
                        }

                        ListItem {
                            height: configLayout.height
                            divider.visible: false
                            visible: !addressbook.collections.count
                            ListItemLayout {
                                id: configLayout
                                title.text: qsTr("Add Collection")
                            } 
                        }

                        Repeater {
                            model: addressbook ? addressbook.collections : null
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
                                    ProgressionSlot {}
                                }
                                onClicked: ContactActions.setSelectedAddressBook(model.index)
                                divider.visible: false

                                Line {
                                    anchors {
                                        right: parent.right
                                        bottom: parent.bottom
                                        left: parent.left
                                        leftMargin: units.gu(7)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
