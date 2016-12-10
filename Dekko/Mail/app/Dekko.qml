import QtQuick 2.4
import QtQuick.Window 2.1
import QuickFlux 1.0
import PlugMan 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Views 1.0

Item {
    id: root

    ItemRegistry {
        target: root
        location: "Dekko::Mail"
        loadMode: ItemRegistry.LoadFirstEnabled
    }

    AppListener {
        waitFor: [ViewStore.listenerId]
        Filter {
            type: ViewKeys.openComposer
            onDispatched: {
                if (d.composeWindow === null) {

                } else {
                    // Raise current compose window
                    // show message about composition already in progress
                    // do they want to save draft etc.
                }
            }
        }

        Filter {
            type: ViewKeys.closeComposer
            onDispatched: {
                if (d.composeWindow !== null) {
                    d.composeWindow.destroy()
                }
            }
        }
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

    Component {
        id: compoWindow;
        DevWindow {}
    }
    QtObject {
        id: d
        property Window composeWindow : null
    }
}
