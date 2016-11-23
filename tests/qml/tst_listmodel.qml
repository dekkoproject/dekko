import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

TestCase {
    id: test
    width: 500; height: width
    name: "MazDB::ListModel"
    property string source: Qt.resolvedUrl("~/.maztest.db")

    MazDB {
        id: db
    }

    MazDBListModel {
        id: dbModel
    }

    function initTestCase() {
        db.source = source
        dbModel.source = source
    }

    function cleanupTestCase() {
        dbModel.close()
        db.destroyDB(test.source)
    }

    function init() {
        compare(
            db.batch()
                .put("/comics/","asdf")
                .put("/comics/~", "asdf")
                .put("list", ["asdf"])
                .put("/comics/abc", {"a": "b"})
                .write(), true, "Batch Operation Failed"
            )
    }

    function cleanup() {
        dbModel.range.start = ""
        dbModel.range.end = ""
        dbModel.excludeKeys = []
        dbModel.filter = ""
    }

    function test_add_remove_values() {
        compare(db.opened, true, db.errorString)
        compare(dbModel.count, 4, "Epic fail")
        compare(db.del("list"), true)
        compare(dbModel.count, 3, " list Should have been removed")
        compare(db.del("/comics/"), true)
        compare(dbModel.count, 2, "/comics/ Should have been removed")
    }

    function test_filter() {
        compare(db.opened, true, db.errorString)
        compare(dbModel.count, 4, "Epic fail")
        dbModel.setFilter("/comics/")
        compare(dbModel.count, 3, "Should only be 1 row")
        dbModel.setFilterPolicy(MazDBListModel.ExactMatch)
        dbModel.setFilter("/comics/")
        compare(dbModel.count, 1, "Should only be 1 now")
    }

    function test_range() {
        compare(db.opened, true, db.errorString)
        dbModel.range.start = "/comics/"
        dbModel.range.end = "/comics/~"
        dbModel.setExcludeKeys([dbModel.range.start, dbModel.range.end])
        compare(dbModel.count, 1, "/comics/abc Should be the only one")
    }
}
