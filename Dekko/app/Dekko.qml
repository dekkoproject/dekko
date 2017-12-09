import QtQuick 2.4
import QtQuick.Window 2.1
import QuickFlux 1.0
import PlugMan 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Mail 1.0

Item {
    id: root

    AppListener {
        enabled: true

        Filter {
            type: ViewKeys.openComposer
            onDispatched: {

                if (d.composeWindow === null) {
                    var comp = d.getComponent("Dekko::Mail::Composer")
                    if (comp !== null) {
                        d.composeWindow = comp.createObject(root)
                        MailStore.disableMsgListUpdates = true;
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
                MailStore.disableMsgListUpdates = false;
            }
        }

        Filter {
            type: ViewKeys.openSettings
            onDispatched: {
                console.log("Open Settings")
                if (d.settingsWindow === null) {
                    var comp = d.getComponent("Dekko::Mail::Settings")
                    if (comp !== null) {
                        d.settingsWindow = comp.createObject(root)
                    }
                } else {
                    d.settingsWindow.raise()
                }
            }
        }

        Filter {
            type: ViewKeys.closeSettings
            onDispatched: {
                if (d.settingsWindow !== null) {
                    d.settingsWindow.destroy()
                }
            }
        }

        Filter {
            type: ViewKeys.runSetupWizard
            onDispatched: {
                console.log("Open Mail Setup Wizard")
                if (d.setupWizard === null) {
                    var comp = d.getComponent("Dekko::Mail::SetupWizard")
                    if (comp !== null) {
                        d.setupWizard = comp.createObject(root)
                    }
                } else {
                    d.setupWizard.raise()
                }
            }
        }

        Filter {
            type: ViewKeys.closeSetupWizard
            onDispatched: {
                console.log("Closing setup wizard")
                if (d.setupWizard !== null) {
                    d.setupWizard.destroy()
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
        property Window setupWizard: null

        function getComponent(location) {
            var src = registry.findFirstEnabled(location)
            if (src) {
                var comp = Qt.createComponent(Qt.resolvedUrl("file:///" + src))
                if (comp.status === Component.Error) {
                    console.log("Failed to create component: ", comp.errorString())
                    return null
                }
                return comp
            }
        }
    }
}
