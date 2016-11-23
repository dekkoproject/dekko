import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

TestCase {
    id: test
    name: "MazDB::Settings"

    MazDBSettings {
        id: settings
        property string property1: "Hello"
        property string property2: "World"
        readonly property int readonlyproperty3: 1000
    }

    MazDB {
        id: db
    }

    function initTestCase() {
        db.source = settings.fileName()
    }

    function cleanupTestCase() {
        settings.close()
        db.destroyDB(db.source)
    }

    function test_settings() {
        compare(db.opened, true, db.errorString)
        compare(db.get("property1"), "Hello")
        compare(db.get("property2"), "World")
        compare(db.get("readonlyproperty3"), undefined)
    }

    function test_overwrite() {
        compare(db.opened, true, db.errorString)
        compare(db.get("property1"), "Hello")
        compare(db.putSync("property1", "Foo"),true)
        compare(settings.property1, "Foo")
        compare(db.putSync("property1", "Hello"), true)
        compare(db.get("property1"), "Hello")
    }

}
