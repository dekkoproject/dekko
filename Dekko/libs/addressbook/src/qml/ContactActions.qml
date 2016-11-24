pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

ActionCreator {

    /*! Signal emitted when the AddressBooks service is ready for registrations */
    signal registerAddressBooks()

    /*!
     * A collection was successfully registered
     */
    signal collectionRegistered(string name, string collection)

    signal collectionRemoved(string name, string collection)

    /*!
      * Retrieval of all contacts for an addressbook has been requested
      *
      * Application plugins will only receive this if they decalre they support syncing
      */
    signal syncCollection(string name, string collection)

    /*!
     * Dispatched when an addressbook instance has been initialized
     *
     * Access to the addressbook is now possible using it's name
     */
    signal addressBookReady(string name)

    signal addressBookRemoved(string name)

    /*!
     * Add Contact
     *
     */
    signal addContact(string name, string collection, var contact)

    signal deleteContact(string name, string key)

    signal updateContact(string name, string key, var contact)

    /*!
     * Register an addressBook:
     */
    function registerAddressBook(config) {
        AppDispatcher.dispatch("registerAddressBook", {config: config})
    }
    /*!
     * Register an addressbook collection
     */
    function registerCollection(name, collection) {
        AppDispatcher.dispatch("registerCollection", {name: name, collection: collection})
    }

    function removeCollection(name, collection) {
        AppDispatcher.dispatch("removeCollection", {name: name, collection: collection})
    }
}
