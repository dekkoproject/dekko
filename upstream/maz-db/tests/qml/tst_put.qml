import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

TestCase {
    name: "MazDB::PutGet"
    property string source: Qt.resolvedUrl("~/.maztest.db")

    MazDB {
        id: db
    }

    Rectangle {
        id: rect
        visible: false
        color: "red"
    }

    function initTestCase() {
        db.source = source
    }

    function cleanupTestCase() {
        db.destroyDB(source)
    }

    function test_put_get_string() {
        compare(db.opened, true, db.errorString)
        compare(db.putSync("string", "asdf"), true, "should be ok")
        compare(db.get("string"), "asdf")
    }

    function test_put_get_int() {
        compare(db.opened, true, db.errorString)
        compare(db.putSync("int", 10), true, "should be ok")
        compare(db.get("int"), 10)
    }

    function test_put_get_double() {
        compare(db.opened, true, db.errorString)
        compare(db.putSync("double", 1.0), true, "should be ok")
        compare(db.get("double"), 1.0)
    }

    function test_put_get_bool() {
        compare(db.opened, true, db.errorString)
        compare(db.putSync("bool", true), true, "should be ok")
        compare(db.get("bool"), true)
    }

    function test_put_get_list() {
        compare(db.opened, true, db.errorString)
        compare(db.putSync("list", ["fdfd", "gpgp"]), true, "should be ok")
        compare(db.get("list"), ["fdfd", "gpgp"])
    }

    function test_put_get_object() {
        compare(db.opened, true, db.errorString)
        compare(db.putSync("object", {a: "a", b: "b"}), true, "should be ok")
        compare(db.get("object"), {a: "a", b: "b"})
    }

    function test_put_get_date() {
        compare(db.opened, true, db.errorString)
        var date = Date.now()
        compare(db.putSync("date", date), true, "should be ok")
        compare(db.get("date"), date)
    }

    function test_put_get_time() {
        compare(db.opened, true, db.errorString)
        var time = new Date()
        compare(db.putSync("time", time.getTime()), true, "should be ok")
        compare(db.get("time"), time.getTime())
    }
}

