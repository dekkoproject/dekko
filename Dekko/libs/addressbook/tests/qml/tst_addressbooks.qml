import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import Dekko.Contacts 0.1

Item {
    id: test
    width: 640
    height: 480

    property var abookConfig: {
                "name": validName,
                "path": "/tmp/path",
                "displayName": "AddressBook 1",
                "settings": "",
                "options": {
                    "canRead": true,
                    "canWrite": false,
                    "canDelete": false,
                    "canSync": true,
                    "multipleCollections": true
                }
    }

    property string validName: "abook1"

    AddressBooks {
        id: abooks
    }

    TestCase {
        name: "AddressBooks"

        function initTestCase() {
            abooks.registerAddressBook(abookConfig)
        }

        function test_create() {
            compare(abooks.count, 1, "Expected 1 addressbook")
            compare(abooks.model.getFirst().displayName, "AddressBook 1")
            compare(abooks.model.getFirst().canRead, true)
            compare(abooks.model.getFirst().canWrite, false)
            compare(abooks.model.getFirst().canDelete, false)
            compare(abooks.model.getFirst().canSync, true)
        }

        function test_collection() {
            compare(abooks.count, 1)
            abooks.registerCollection(validName, "New Collection")
            compare(abooks.model.getFirst().collections.count, 1)
            compare(abooks.model.getFirst().collections.getFirst().name, "New Collection")
            compare(abooks.model.getFirst().collections.getFirst().source, "/tmp/path")
            compare(abooks.model.getFirst().collections.getFirst().startKey, "New Collection")
            compare(abooks.model.getFirst().collections.getFirst().endKey, "New Collection~")
        }
    }
}
