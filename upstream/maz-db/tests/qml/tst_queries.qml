import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import MazDB 1.0

TestCase {
    id: test
    name: "MazDB::Queries"
    property string source: Qt.resolvedUrl("~/.maztest.db")

    MazDB {
        id: db
    }

    MazDBQueryEngine {
        id: query
        query: MazDBQuery {
            definition: [
                WHERE {
                    id: where
                },
                OR {
                    id: or
                },
                AND {
                    id: and
                }
            ]
        }
    }

    function initTestCase() {
        db.source = test.source
        query.source = test.source
    }

    function cleanupTestCase() {
        query.close()
        db.destroyDB(test.source)
    }


    function cleanup() {
        where.property = ""
        where.value = undefined
        where.comparitor = undefined
        or.property = ""
        or.value = undefined
        or.comparitor = undefined
        and.property = ""
        and.value = undefined
        and.comparitor = undefined
        db.del("data")
    }

    readonly property var dataMap: {
        "a" : { "b" : {"c" : "hello"} },
        "d" : { "e" : {"f" : "world"} }
    }

    function test_object_paths() {
        compare(db.putSync("data", dataMap), true)
        where.property = "a.b.c"
        where.value = "hello"
        and.property = "d.e.f"
        and.value = "world"

        var count = 0
        query.queryStream(function(key, value) {
            compare(key, "data")
            compare(value.a.b.c, "hello")
            ++count
            return true;
        })
        verify(count === 1)
    }

    function test_where_or() {
        compare(db.putSync("data", "foo"), true)
        compare(db.putSync("data1", "bar"), true)

        where.value = "foo"
        or.value = "bar"

        var count = 0
        query.queryStream(function(key, value) {
            ++count
            return true;
        })
        verify(count, 2)
        db.del("data1")
    }

    function test_custom_comparitor() {
        compare(db.putSync("data", "working"), true)
        where.comparitor = Qt.binding(customComparitor)
        var count = 0
        query.queryStream(function(key, value) {
            ++count
            return true;
        })
        verify(count, 1)
    }

    function customComparitor(value) {
        return value === "working"
    }

}
