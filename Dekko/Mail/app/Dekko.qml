import QtQuick 2.4
import PlugMan 1.0

Item {
    id: root

    ItemRegistry {
        target: root
        location: "Dekko::Mail"
        loadMode: ItemRegistry.LoadFirstEnabled
    }
}
