import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

TestCase {
    id: test
    name: "MazDB::Batch"
    property string source: Qt.resolvedUrl("~/.maztest.db")

    MazDB {
        id: db
    }

    function initTestCase() {
        db.source = test.source
    }

    function cleanupTestCase() {
        db.destroyDB(test.source)
    }

    function test_batch(){
        compare(db.opened, true, db.errorString)
        var status = db.batch()
        .put("asdf","asdf")
        .put("string", "asdf2")
        .put("list", ["asdf"])
        .put("object", {"a": "b"})
        .del("asdf")
        .write()
        compare(status, true, "Batch Operation Failed")
        compare(db.get("asdf"), undefined)
        compare(db.get("string"),"asdf2", "Wrong data")
        compare(db.get("list"),["asdf"], "Wrong data")
        compare(db.get("object"),{"a": "b"}, "Wrong data")
    }
}

