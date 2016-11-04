import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Contacts 1.0
import Plugger 1.0

/*!
 * ContactsWorker
 *
 * The ContactsWorker does the heavy lifting for the contacts plugin.
 * it takes control of loading the backend plugins and populating the
 * addressbooks. It is also where handling of all ContactActions are handled
 *
 * By default the contacts loaders are disabled and won't load any contacts until
 * the enabled property of the ContactsWorker is set to true
 */
AppListener {
    id: worker

    signal pluginsReady()

    waitFor: [ContactsStore.listenerId]

    Filter {
        type: ContactKeys.registerAddressBook
        onDispatched: {
            ContactsStore.addressBooks.registerAddressBook(message.config)
        }
    }

    Filter {
        type: ContactKeys.registerCollection
        onDispatched: {
            ContactsStore.addressBooks.registerCollection(message.name, message.collection)
        }
    }

    Filter {
        type: ContactKeys.removeCollection
        onDispatched: {
            ContactsStore.addressBooks.removeCollection(message.name, message.collection)
        }
    }

    //Load Plugins registry
    AddressBookRegistry{
        onPluginsReady: worker.pluginsReady()
    }

    // TODO: onPluginsLoaded: ContactKeys.registerAddressBooks
}
