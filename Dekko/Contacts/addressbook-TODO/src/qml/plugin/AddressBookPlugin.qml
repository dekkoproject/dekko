import QtQuick 2.4
import QuickFlux 1.0
import MazDB 1.0
import Dekko.Contacts 1.0
import Dekko.Components 1.0

AppListener {

    // name used to identify this plugin
    property string name: ""
    // Displayed in the main UI
    property string displayName: qsTr("Example Addressbook")
    property string settings: ""
    property bool canRead: true
    property bool canWrite: false
    property bool canDelete: false
    property bool canSync: false
    property bool multipleCollections: false
    property QtObject options: QtObject {
        id: opts
        // plugin supports reading

    }
    property alias addressbook: db
    readonly property string addressbookPath: name ? Qt.resolvedUrl("%1/contacts/plugins/%2.db".arg(Paths.cachePath).arg(name)) : ""

    readonly property var config: {
        "name" : name,
        "displayName" : displayName,
        "path" : addressbookPath,
        "settings" : settings,
        "options" : {
            "canRead": canRead,
            "canWrite": canWrite,
            "canDelete": canDelete,
            "canSync": canSync,
            "multipleCollections": multipleCollections
        }
    }

    MazDB {
        id: db
        source: addressbookPath
        onSourceChanged: console.log("AddressBookPath is: ", addressbookPath)

        // Convenience method for creating a contact key for a collection
        // otherwise you have to format the key yourself.
        function key(collection, id) {
            return "%1/%2".arg(collection).arg(id)
        }

        function nextUid(collection) {
            var next = db.get(collection, 0) + 1
            db.put(collection, next)
            return next
        }

        function addCollection(collection) {
            db.batch()
            .put(collection, 0)
            .put(collection + "~", {})
            .write()
        }

        function insertContact(id, contact) {
            db.put(id, contact)
        }

        function deleteContact(id) {
            db.del(id)
        }

        function move(oldKey, newKey) {
            var contact = db.get(oldKey);
            db.batch()
            .del(oldKey)
            .put(newKey, contact)
            .write()
        }

        // this is a convenience method for easy reading
        // in a plugin.
        function updateContact(id, contact) {
            db.put(id, contact)
        }

        function collectionStream(collection, cb) {
            var startKey = collection
            var endKey = "%1~".arg(collection)
            db.rangeStream(cb, startKey, endKey)
        }
    }

    Event {
        type: ContactKeys.registerAddressBooks
        onDispatched: {
            ContactActions.registerAddressBook(config)
        }
    }
}
