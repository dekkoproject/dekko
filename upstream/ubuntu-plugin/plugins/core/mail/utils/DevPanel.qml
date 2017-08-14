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
import QtQuick.Window 2.2
import Dekko.Components 1.0
import Dekko.Mail 1.0
import Ubuntu.Components 1.3
import Ubuntu.PerformanceMetrics 1.0
import Dekko.Ubuntu.Components 1.0
import Dekko.Ubuntu.Constants 1.0

Rectangle {
    id: devPanel
    anchors.fill: parent
    color: Style.devPanelBackground
    clip: true
    StretchRow {
        anchors.fill: parent
        spacing: 0
        PanelContainer {
            resizable: true
            minSize: units.gu(25)
            maxSize: units.gu(40)
            height: parent.height
            activeEdge: Item.Right
            StretchColumn {
                anchors.fill: parent
                Stretcher {
                    implicitWidth: parent.width
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    ListView {
                        id: sq
                        anchors {
                            fill: parent
                            margins: units.gu(0.5)
                        }
                        clip: true
                        header: Component {
                            Item {
                                width: parent.width
                                height: units.gu(3)
                                Label {
                                    anchors.centerIn: parent
                                    text: "Service actions"
                                    color: UbuntuColors.orange
                                }
                            }
                        }
                        model: Client.service.serviceActionQueue
                        delegate: Label {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            text: model.qtObject.description
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            color: UbuntuColors.porcelain
                        }
                    }
                }
                PanelContainer {
                    width: parent.width
                    activeEdge: Item.Top
                    size: parent.height / 2
                    ListView {
                        anchors {
                            fill: parent
                            margins: units.gu(0.5)
                        }
                        clip: true
                        header: Component {
                            Item {
                                width: parent.width
                                height: units.gu(3)
                                Label {
                                    anchors.centerIn: parent
                                    text: "Undoable actions"
                                    color: UbuntuColors.green
                                }
                            }
                        }
                        model: Client.service.undoActionQueue
                        delegate: Label {
                            width: units.gu(25)
                            text: model.qtObject.description
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            color: UbuntuColors.porcelain
                        }
                    }
                }
            }
        }
        PanelContainer {
            resizable: true
            minSize: units.gu(30)
            maxSize: units.gu(60)
            size: units.gu(60)
            height: parent.height
            activeEdge: Item.Right
            ListView {
                id: qmlLog
                anchors.fill: parent
                anchors.margins: units.gu(0.5)
                model: LogRecorder.model
                clip: true
                delegate: Label {
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: model.qtObject.message
                }
                onCountChanged: positionViewAtEnd()
                Connections {
                    target: LogRecorder.model
                    onCountChanged: {
                        if (!logPositionTimer.running)
                            logPositionTimer.start()
                    }
                }
                Timer {
                    id: logPositionTimer
                    interval: 300
                    repeat: false
                    onTriggered: qmlLog.positionViewAtEnd()
                }
            }
        }

        Stretcher {
            implicitHeight: parent.height
            PerformanceOverlay {
                anchors {
                    centerIn: parent
                }
            }

        }
    }
    HeaderButton {
        anchors {
            top: parent.top
            right: parent.right
        }
        height: units.gu(5)
        width: height
        iconName: "view-fullscreen"
        onClicked: detach()
    }

    function detach () {
        if (d.subWindow === null) {
            var rootItem = Introspector.window (devPanel);
            var abspos = rootItem.contentItem.mapFromItem(devPanel, 0 , 0);
            d.subWindow = compoWindow.createObject(Introspector.window(devPanel), {
                                                       "x" : (abspos.x + rootItem.x),
                                                       "y" : (abspos.y + rootItem.y),
                                                   });
            devPanel.parent = d.subWindow.contentItem;
//            pc.visible = false
        }
    }

    function attach () {
        if (d.subWindow !== null) {
            devPanel.parent = pc.container;
//            pc.visible = true
            d.subWindow.destroy();
        }
    }

    Component {
        id: compoWindow;
        DevWindow {}
    }
    QtObject {
        id: d
        property Window subWindow : null
    }
}

