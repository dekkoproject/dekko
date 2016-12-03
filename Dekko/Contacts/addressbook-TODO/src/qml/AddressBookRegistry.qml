import QtQuick 2.4
import Dekko.Contacts 1.0
import Plugger 1.0

Item {
    id: pluginContainer

    signal pluginsReady()

    ItemRegistry {
        target: pluginContainer
        location: "Dekko::AddressBook::Provider"
        loadMode: ItemRegistry.LoadAll
        Component.onCompleted: delayInit.start()
    }
    Timer {
        id: delayInit
        interval: 2000
        repeat: false
        onTriggered: pluginContainer.pluginsReady()
    }
}
