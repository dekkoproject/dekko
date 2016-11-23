import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

TestCase {
    id: test
    name: "MazDB::ReadStream"
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

    function test_read_stream(){
        var data = {
            "asdf":"asdf",
            "string": "asdf2",
            "list": ["asdf"],
            "object": {"a": "b"},
            "c": "c",
            "d": "d"
        }

        db.source = "read_stream.db"
        compare(db.opened, true, db.errorString)
        Object.keys(data).forEach(function (key){
            db.put(key, data[key])
        })
        db.readStream(function (key,value){
            compare(value, data[key], "Inconsistent data")
            return true;
        })
        db.source = ""
        compare(db.destroyDB("read_stream.db"), true, db.errorString)
    }

    function test_read_stream_with_key(){
        var data = {
            "asdf":"asdf",
            "string": "asdf2",
            "list": ["asdf"],
            "object": {"a": "b"},
            "c": "c",
            "d": "d"
        }

        db.source = "read_stream.db"
        compare(db.opened, true, db.errorString)
        Object.keys(data).forEach(function (key){
            db.put(key, data[key])
        })
        db.readStream(function (key,value){
            compare(value, data["string"], "Inconsistent data")
            return true;
        }, "string") // we only expect the "string" value to be returned.
        db.source = ""
        compare(db.destroyDB("read_stream.db"), true, db.errorString)
    }
}

