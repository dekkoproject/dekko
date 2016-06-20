/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.4
import QtGraphicalEffects 1.0
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../../components"
import "../../../stores/accounts"
import "../../../actions/wizard"
import "../../../constants"

DekkoPage {
    id: noAccounts
    objectName: "addAnotherUI"
    pageHeader.title: qsTr("Success")

    PageContent {

        ScrollView {
            anchors.fill: parent

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
                        name: Icons.TickIcon
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
                        topMargin:Style.defaultSpacing
                    }
                    height: t.height

                    Label {
                        id: t
                        anchors {
                            top: parent.top
                            horizontalCenter: parent.horizontalCenter
                        }
                        width: parent.width - units.gu(4)
                        text: qsTr("Would you like to add another account?")
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        fontSize: "large"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                Button {
                    id: b
                    text: qsTr("Add another")
                    color: UbuntuColors.green
                    anchors {
                        top: l.bottom
                        topMargin: units.gu(3)
                        horizontalCenter: parent.horizontalCenter
                    }
                    onClicked: WizardActions.addAnotherAccount()
                }
                Button {
                    id: b2
                    text: qsTr("Not now")
                    color: UbuntuColors.green
                    anchors {
                        top: b.bottom
                        topMargin: units.gu(3)
                        horizontalCenter: parent.horizontalCenter
                    }
                    onClicked: WizardActions.endSetup()
                }
            }
        }
    }
}
