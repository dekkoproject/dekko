import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

Item {
    id: test
    width: 640
    height: 480

    MazDB {
        id: db
    }

    MazDB {
        id: db2
    }

    TestCase {
        name: "MazDB"

        property string source: Qt.resolvedUrl("~/.maztest.db")

        function initTestCase() {
            db.source = source
        }

        function cleanupTestCase() {
            db.destroyDB(source)
        }

        function test_create() {
            compare(db.opened, true, db.errorString)
        }

        function test_destroy() {
            db2.source = "/tmp/test2.db"
            compare(db2.opened, true, db2.errorString)
            compare(db2.destroyDB("/tmp/test2.db"), true, db.errorString)
        }

        function test_get_default(){
            compare(db.opened, true, db.errorString)
            compare(db.get("invalidKey", [{"id": "a"}]), [{"id": "a"}])
        }

        function test_delete(){
            compare(db.opened, true, db.errorString)
            compare(db.put("asdf", ["asdf"]), true)
            compare(db.del("asdf"), true)
            compare(db.get("asdf"), undefined)
        }
    }
}
