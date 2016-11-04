import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Contacts 1.0
import Dekko.Contacts.Plugin 1.0

AddressBookPlugin {
    id: plugin

    // You must provide the name for your plugin
    name: "default"
    displayName: qsTr("Internal")

    canRead: true
    canWrite: true
    canDelete: true
    canSync: false
    multipleCollections: false

    Event {
        type: ContactKeys.addressBookReady
        onDispatched: {
            // Make sure we only work on events for this plugin.
            // All events contain the plugin name so observers
            // can filter accordingly
            if (message.name === name) {
                // we can register our collections now
                var collections = getCollections()
                for (var i in collections) {
                    ContactActions.registerCollection(plugin.name, collections[i])
                }
            }
        }
    }

    Event {
        type: ContactKeys.collectionRegistered
        onDispatched: {
            if (message.name === name) {
                // This event has little meaning as you can write to the addressbook at anytime,
                // but it could be handy if you want to trigger a sync after everythings setup, or populate
                // the addressbook for the first time.
                // NOTHING TO DO
            }
        }
    }

    Event {
        type: ContactKeys.collectionRemoved
        onDispatched: {
            if (message.name === name) {
                // The collection has been removed by the user
                // You should delete all entries in the addressbook and cleanup
                // any collection specific settings
                var batch = addressbook.batch()
                addressbook.collectionStream(message.collection, function (key, value) {
                    batch.del(key);
                    return true;
                })
                batch.write()
            }
        }
    }

    Event {
        type: ContactKeys.syncCollection
        onDispatched: {
            if (message.name === name && options.canSync) {
                // Perform a sync with the remote/local resource
            }
        }
    }

    Event {
        type: ContactKeys.addContact
        onDispatched: {
            if (message.name === name) {
                // you can check, modify, whatever the contact object
                // before commiting it to the addressbook
                var nextKey = addressbook.key(message.collection, addressbook.nextUid())
                addressbook.insertContact(nextKey, message.contact)
            }
        }
    }

    Event {
        type: ContactKeys.deleteContact
        onDispatched: {
            if (message.name === name) {
                addressbook.deleteContact(message.key)
            }
        }
    }

    Event {
        type: ContactKeys.updateContact
        onDispatched: {
            if (message.name === name) {
                // You can modify the contact object to however you want
                // before updating your store
                addressbook.updateContact(message.key, message.contact)
            }
        }
    }

    function getCollections() {
        return [qsTr("Default")]
    }
}
