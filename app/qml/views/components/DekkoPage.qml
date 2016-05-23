import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import Dekko.Settings 1.0
import QuickFlux 1.0
import "../models"
import "../../actions/views"
import "../../constants"

Item {
    id: dekkoPage

    property alias pageHeader: h
    property alias selectedIndex: h.selectedIndex
    property alias isSearchMode: h.isSearchMode
    property alias extendHeader: h.extend

    default property alias content: mainContent.data

    property BottomEdgeConfiguration bottomEdgeConfig: null
    property DrawerConfiguration drawerConfig: null

    signal searchActivated(string searchString)
    signal searchCanceled()
    signal multiSelectCanceled()

    function startMultiSelect() {
        h.startMultiSelect()
    }
    clip: true

    DekkoHeader {
        id: h
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        onSearchActivated: dekkoPage.searchActivated(searchString)
        onSearchCanceled: dekkoPage.searchCanceled()
        onMultiSelectCanceled: dekkoPage.multiSelectCanceled()
        onComposeNewMessage: ViewActions.openMessageComposer()
    }

    PixelPerfectItem {
        id: mainContent
        anchors {
            left: parent.left
            top: h.bottom
            right: parent.right
            bottom: parent.bottom
            bottomMargin: bel.active && bel.item.hint.visible ? bel.item.hint.height : 0
        }
        // MAIN CONTENT HERE
    }

    Loader {
        id: bel
        anchors.fill: parent
        active: bottomEdgeConfig !== null && bottomEdgeConfig.enabled
        onActiveChanged: console.log("ACTIVE CHANGED: ", active)
        asynchronous: true
        sourceComponent: Component {
            BottomEdge {
                id: bottomEdge
                height: dekkoPage.height
                hint.visible: bottomEdgeConfig.hintVisible
                hint.text: bottomEdgeConfig.hintText
                visible: bottomEdgeConfig.enabled
                hint.iconSource: bottomEdgeConfig.iconSource
//                hint.flickable: bottomEdgeConfig.enabled ? bottomEdgeConfig.flickable : null
                contentComponent: bottomEdgeConfig.sourceComponent
                Component.onCompleted: QuickUtils.mouseAttached = true

                AppListener {
                    Filter {
                        type: ViewKeys.openMessageComposer
                        onDispatched: {
                            if (bottomEdgeConfig.canActionTrigger) {
                                bottomEdge.commit()
                            }
                        }
                    }
                    Filter {
                        type: ViewKeys.replyToOpenMessage
                        onDispatched: {
                            if (bottomEdgeConfig.canActionTrigger) {
                                bottomEdge.commit()
                            }
                        }
                    }
                }
            }
        }
    }

    property alias navDrawer: loader.item
    NavigationSettings {
        id: navSettings
    }

    Component {
        id: navDrawerCompo
        NavigationDrawer {
            id: navDrawer
            parent: dekkoPage
            animate: true
            width: Style.defaultPanelWidth
            state: "fixed"
            // make sure this overlays the page contents
            z: 10
            anchors {
                left: parent.left
                top: h.bottom
                bottom: parent.bottom
            }
            panelModel: NavMenuModel{}
        }

    }
    Loader {
        id: loader
        anchors {
            left: parent.left
            top: h.bottom
            bottom: parent.bottom
        }
        active: drawerConfig !== null && drawerConfig.enabled
        z: 10
        asynchronous: true
        sourceComponent: navDrawerCompo
    }
}
