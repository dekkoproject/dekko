import QtQuick 2.4
import Ubuntu.Components 1.3
import "../../constants"

// Panel container has multiple uses. It can be used as:
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
    readonly property bool expanded: !d.collapsed
    default property alias content: internal.data

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

        //        Rectangle {
        //            id: grip;
        //            gradient: (handle.pressed
        //                       ? Style.gradientShaded (Style.colorHighlight, Style.colorNone)
        //                       : Style.gradientShaded (Style.colorClickable, Style.colorNone));
        //            anchors.centerIn: parent;
        //        }
    }

    Line {
        id: divider
        visible: grabber.visible
    }

    QtObject {
        id: d
        property bool collapsed: false
        readonly property bool shouldStretch: dekko.viewState.isSmallFF && stretchOnSmallFF
        readonly property bool dockedLeft: activeEdge === Item.Right
        readonly property bool dockedRight: activeEdge === Item.Left
        readonly property bool dockedBottom: activeEdge === Item.Top
        readonly property bool dockedTop: activeEdge === Item.Bottom
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
                width: Style.defaultSpacing;
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
                width: Style.defaultSpacing;
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
                height: Style.defaultSpacing;
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
                height: Style.defaultSpacing;
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

