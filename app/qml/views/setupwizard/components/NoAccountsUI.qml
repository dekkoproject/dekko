import QtQuick 2.4
import QtGraphicalEffects 1.0
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../../components"
import "../../../stores"

DekkoPage {
    id: noAccounts
    objectName: "noAccountsUI"
    pageHeader.title: qsTr("Accounts")
    signal createAccount()
    signal quit()

    ScrollView {
        anchors {
            left: parent.left
            top: pageHeader.bottom
            right: parent.right
            bottom: parent.bottom
        }

        Flickable {
            anchors.fill: parent
            contentHeight: ibxContainer.height + b.height + l.height + units.gu(7)
            clip: true

            Item {
                id:ibxContainer
                width: parent.width
                height: inbox.height + units.gu(5)


                InnerShadow {
                    anchors.fill: inbox
                    radius: 8.0
                    samples: 16
                    horizontalOffset: -3
                    verticalOffset: 3
                    color: UbuntuColors.ash
                    source: inbox
                }

                CachedImage {
                    id: inbox
                    name: Icons.AccountIcon
                    height: units.gu(20)
                    width: height
                    anchors {
                        bottom: parent.bottom
                        horizontalCenter: parent.horizontalCenter
                    }

                    color: UbuntuColors.silk
                    visible: false
                }
            }

            Item {
                id: l
                anchors {
                    left: parent.left
                    right:parent.right
                    top: ibxContainer.bottom
                    topMargin:units.gu(2)
                }
                height: t.height

                Label {
                    id: t
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                    width: parent.width - units.gu(4)
                    text: qsTr("No email account is setup.")
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    fontSize: "large"
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Button {
                id: b
                text: qsTr("Add now")
                color: UbuntuColors.green
                anchors {
                    top: l.bottom
                    topMargin: units.gu(3)
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: AccountSetup.createAccount()
            }
        }
    }
}
