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
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../../constants"

Item {
    id: panel
    width: implicitWidth;
    height: implicitHeight;
    //    visible: !detached;
    implicitWidth: (!d.shouldStretch ? size : -1);
    implicitHeight: (!d.shouldStretch ? size : -1);
    // Which edge should have the resizable mouse area etc
    // Defaults to right side as if it is docked to the left
    // of it's parent
    property int activeEdge: Item.Right
    property int size: units.gu(30)
    property int minSize: units.gu(10)
    property int maxSize: units.gu(50)
    property bool resizable: true
    // If true the panels implicit size hints will get set to -1
    // which when used in a StretchRow/Column will fill all the available space.
    // This works the same was as using Stretcher{}
    // You only really want to set this to true if this panel is the main page container
    // for mobile / small form factor devices.
    property bool stretchOnSmallFF: false
    property var viewState: dekko ? dekko : undefined
    readonly property bool expanded: !d.collapsed
    default property alias content: internal.data
    property alias contentItem: internal
    property alias container: container

    Item {
        id: container;
        visible: expanded;
        anchors.fill: parent
        Item {
            id: internal;
            anchors.fill: parent;

            // CONTENT HERE
        }
    }

    MouseArea {
        id: grabber;
        visible: (expanded && resizable);
        onPressed: {
            var tmp = mapToItem (panel.parent, mouse.x, mouse.y);
            originalPos  = Qt.point (tmp.x, tmp.y);
            originalSize = size;
        }
        onPositionChanged: {
            var absCurrPos = mapToItem(panel.parent, mouse.x, mouse.y);
            var deltaX = (absCurrPos.x - originalPos.x);
            var deltaY = (absCurrPos.y - originalPos.y);
            var tmp = originalSize;
            switch(activeEdge) {
            case Item.Top:
                tmp -= deltaY;
                break;
            case Item.Left:
                tmp -= deltaX;
                break;
            case Item.Right:
                tmp += deltaX;
                break;
            case Item.Bottom:
                tmp += deltaY;
                break;
            }
            size = Math.max(minSize, Math.min (maxSize, tmp));
        }

        property int originalSize : 0;
        property point originalPos : Qt.point(0,0);

        HorizontalGradiant {
            id: sideShadow
            visible: grabber.pressed
            anchors {
                right: parent.right
            }
            rightToLeft: true
            height: parent.height
            z: 1
            opacity: 0.7
            width: units.gu(0.5)
            gradient: Gradient {
                GradientStop { position: 0.0; color: UbuntuColors.blue }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }
    }

    Line {
        id: divider
        visible: grabber.visible
    }

    function detach () {
        if (d.subWindow === null) {
            var rootItem = Introspector.window (panel);
            var abspos = rootItem.contentItem.mapFromItem (panel, 0 , 0);
            d.subWindow = compoWindow.createObject(Introspector.window (panel), {
                                                       "x" : (abspos.x + rootItem.x),
                                                       "y" : (abspos.y + rootItem.y),
                                                   });
            panel.parent = d.subWindow.contentItem;
        }
    }

    function attach () {
        if (d.subWindow !== null) {
            panel.parent = internal;
            d.subWindow.destroy ();
        }
    }
    Component {
        id: compoWindow;

        Window {
//            color: rect.color;
            title: "Dev Stuff"
            width: internal.width;
            height: internal.height;
            visible: true;
            onClosing: { attach (); }
        }
    }

    QtObject {
        id: d
        property bool collapsed: false
        readonly property bool shouldStretch: viewState.isSmallFF && stretchOnSmallFF
        readonly property bool dockedLeft: activeEdge === Item.Right
        readonly property bool dockedRight: activeEdge === Item.Left
        readonly property bool dockedBottom: activeEdge === Item.Top
        readonly property bool dockedTop: activeEdge === Item.Bottom
        property Window subWindow : null
    }

    states: [
        State {
            when: d.dockedLeft

            AnchorChanges {
                target: grabber;
                anchors {
                    top: panel.top;
                    left: undefined;
                    right: panel.right;
                    bottom: panel.bottom;
                }
            }
            PropertyChanges {
                target: grabber;
                width: units.gu(0.5);
                cursorShape: Qt.SizeHorCursor;
            }
            AnchorChanges {
                target: divider;
                anchors {
                    top: panel.top;
                    left: undefined;
                    right: panel.right;
                    bottom: panel.bottom;
                }
            }
        },
        State {
            when: d.dockedRight

            AnchorChanges {
                target: grabber;
                anchors {
                    top: panel.top;
                    left: panel.left;
                    right: undefined;
                    bottom: panel.bottom;
                }
            }
            PropertyChanges {
                target: grabber;
                width: units.gu(0.5);
                cursorShape: Qt.SizeHorCursor;
            }
            AnchorChanges {
                target: divider;
                anchors {
                    top: panel.top;
                    left: panel.left;
                    right: undefined;
                    bottom: panel.bottom;
                }
            }
        },
        State {
            when: d.dockedTop

            AnchorChanges {
                target: grabber;
                anchors {
                    top: undefined;
                    left: panel.left;
                    right: panel.right;
                    bottom: panel.bottom;
                }
            }
            PropertyChanges {
                target: grabber;
                height: units.gu(0.5);
                cursorShape: Qt.SizeVerCursor;
            }
            AnchorChanges {
                target: divider;
                anchors {
                    top: undefined;
                    left: panel.left;
                    right: panel.right;
                    bottom: panel.bottom;
                }
            }
        },
        State {
            when: d.dockedBottom

            AnchorChanges {
                target: grabber;
                anchors {
                    top: panel.top;
                    left: panel.left;
                    right: panel.right;
                    bottom: undefined;
                }
            }
            PropertyChanges {
                target: grabber;
                height: units.gu(0.5);
                cursorShape: Qt.SizeVerCursor;
            }
            AnchorChanges {
                target: divider;
                anchors {
                    top: panel.top;
                    left: panel.left;
                    right: panel.right;
                    bottom: undefined;
                }
            }
        }
    ]
}

