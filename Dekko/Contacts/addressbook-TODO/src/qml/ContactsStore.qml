pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Contacts 1.0

AppListener {

    property alias addressBooksModel: addressBooks.model
    property alias addressBooks: addressBooks

    AddressBooks {
        id: addressBooks

        onAddressBookRegistered: {
            ContactActions.addressBookReady(name)
        }
        onAddressBookRemoved: {
            ContactActions.addressBookRemoved(name)
        }
        onCollectionRegistered: {
            ContactActions.collectionRegistered(abook, collection)
        }
        onCollectionRemoved: {
            ContactActions.collectionRemoved(abook, collection)
        }
    }
}
