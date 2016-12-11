import QtQuick 2.4
import QtQuick.Window 2.1
import QuickFlux 1.0
import PlugMan 1.0
import Dekko.Mail.API 1.0

Item {
    id: root

    AppListener {
        enabled: true

        Filter {
            type: ViewKeys.openComposer
            onDispatched: {
                if (d.composeWindow === null) {
                    var src = registry.findFirstEnabled("Dekko::Mail::Composer")
                    if (src) {
                        var comp = Qt.createComponent(Qt.resolvedUrl("file:///" + src))
                        if (comp.status === Component.Error) {
                            console.log("Failed to open composer: ", comp.errorString())
                            return
                        }
                        d.composeWindow = comp.createObject(root)
                    }
                } else {
                    d.composeWindow.raise()
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

    ItemRegistry {
        id: registry
        asynchronous: true
        target: root
        location: "Dekko::Mail"
        loadMode: ItemRegistry.LoadFirstEnabled
    }

    QtObject {
        id: d
        property Window settingsWindow: null
        property Window composeWindow: null
    }


//    function detach () {
//        if (d.subWindow === null) {
//            var rootItem = Introspector.window (devPanel);
//            var abspos = rootItem.contentItem.mapFromItem(devPanel, 0 , 0);
//            d.subWindow = compoWindow.createObject(Introspector.window(devPanel), {
//                                                       "x" : (abspos.x + rootItem.x),
//                                                       "y" : (abspos.y + rootItem.y),
//                                                   });
//            devPanel.parent = d.subWindow.contentItem;
////            pc.visible = false
//        }
//    }

//    Component {
//        id: compoWindow;
//        DevWindow {}
//    }

}
