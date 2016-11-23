import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

TestCase {
    id: test
    name: "MazDB::Propogation"
    property string source: Qt.resolvedUrl("~/.maztest.db")

    MazDB {
        id: db
    }

    MazDB {
        id: db2
    }

    SignalSpy {
        id: spy1
        target: db2
        signalName: "keyValueChanged"
    }

    function initTestCase() {
        db.source = test.source
        db2.source = test.source
    }

    function cleanupTestCase() {
        db2.close()
        db.destroyDB(test.source)
    }

    function init() {
        if (spy1.count) {
            spy1.clear()
        }
    }

    function test_propogate_value_changes() {
        compare(db.opened, true, db.errorString)
        compare(db2.opened, true, db2.errorString)
        compare(db.putSync("newkey", "value"), true, "should be ok")
        compare(spy1.count, 1, "Should have only had one keyvalue change")
        compare(db2.get("newkey"), "value", "Data corruption... yuk!")
    }

}

