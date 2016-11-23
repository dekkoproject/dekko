import QtQuick 2.4
import QtQuick.Window 2.1
//import Ubuntu.Components 1.3
import MazDB 1.0

Window {
    id: window
    width: 300
    height: 300
    visible: true

    Item {
        anchors.fill: parent
    }

    /* You can use a MazDBSettings instance like Settings from qt.labs.Settings
     *
     * The database (settings.db) is stored in your applications config directory
     *
     * All declared properties will get reloaded on app start.
     * Readonly properties are not stored in the database so work nice as constants
     *
     * The main thing that this provides over qt.labs.Settings is property change notifications
     * over multiple instances, which is cool!
     */
    MazDBSettings {
        id: settings
        property alias windowWidth: window.width
        property alias windowHeight: window.height
    }

    MazDBSettings {
        id: dbSettings
        // You can seperate settings using categories, just like in qt.labs.Settings
        category: "db-settings"
        // Readonly properties are not stored so cannot be changed in the underlying DB
        readonly property string source: Qt.resolvedUrl("/tmp/maztest4.db")
        property string helloValue: "Heya!! Created via the rolesCallBack"
        property string worldValue: "Or another callback role: "
    }

    /* MazDB is the low level api that interfaces with leveldb
     *
     * You can perform all leveldb actions put()/putSync()/del()/batch()/readStream()/destroyDB()/repairDB()
     *
     * If you have more than one MazDB instance with the same source, key/value changes will
     * get propogated to all instances. This is the basis on how MazDBListModel functions.
     *
     * You can store any QVariant compatible data type in MazDB from primitive types, bytearrays to json objects/arrays or QVariantMap/Hash
     * even any custom c++ types as long as they are QVariant compatible and support data streams.
     * Take a look in MazUtils for all supported types.
     */
    MazDB {
        id: db

        source: dbSettings.source

        // This shows a few ways of interacting with the MazDB api
        Component.onCompleted: {
            // Add multiple key/values in one batch
            db.batch()
                .put("item1","asdf")
                .put("item1/~", "asdf")
                .put("item2", "some other value")
                .put("item1/abc", "What about this?")
                .put("item2/foo","asdf")
                .put("item2/~", "asdf")
                .put("item2/foo/bar", "some other value")
                .put("item1/abc/def", "What about this?")
                .write()

            // Put key/value async
            db.put("async", "async item")

            // Synchronous Put
            var didPut = db.putSync("sync", "Sync item")
            if (didPut) {
                console.log("Synchronous put() success!")
                // get the value we just put in
                console.log("sync value: ", db.get("sync"))
            }

            // If you want to batch write at runtime
            // take a batch instance and run a loop over your data
            var batch = db.batch()
            for (var i = 0; i < 100; ++i) {
                batch.put("loop" + i, "value " + i)
            }
            // Finally write the batch in one go. Pretty nice eh? :-)
            batch.write()

            // You can also read back every key/value we just put in
            // via the readStream api.
            db.readStream(function (key,value){
                console.log("-- ReadStream --")
                console.log("Key: ", key)
                console.log("Value: ", value)
                console.log("----------------")
                // return false if you want to end the readStream early. i.e you found what your looking for
                return true;
            }, "item2") // You can optionally provide a startkey to slice the stream

            // There's also a keyStream, valueStream and rangeStream
            db.keyStream(function(key) {
                console.log("Key: ", key);
                return true;
            })

            db.valueStream(function(value){
                console.log("Val: ", value)
                return true
            })

            db.rangeStream(function (key, value) {
                console.log("RangeKey: ", key)
                return true;
            }, "item1", "loop3") // Only stream keys in this range.

            // also let's call the queryStream
            query.queryStream(function (key, value) {
                console.log("-- Query Stream --")
                console.log("Key: ", key)
                console.log("Value: ", value)
                console.log("------------------")
                return true
            })
        }
    }

    MazDBQueryEngine {
        id: query
        source: dbSettings.source
        query: MazDBQuery {
            // You can define a range to query between
            // If not defined a full scan is performed.
            range {
                start: "item1"
                end: "loop6"
            }
            // Set a limit for the number of results your after.
            // Note: results will be returned based on the lexicographical ordering
            // of the keys.
            limit: 10

            // Define the query to perform
            definition: [
                // All queries MUST contain a WHERE clause
                // which is the first to be evaluated. It doesn't have to be
                // the first in the list it just has to exist :-)
                WHERE {
                    // you can define a property to evaluate. Only works with a subset of types.
                    // QRect/QPoint/QSize/QVariantMap/QVariantHash/QJsonObject
                    property: ""
                    // The value to compare
                    value: "asdf"
                    // Eval operator to use ==, !=, <, >, <=, >=
                    matchRule: Query.Equals
                },
                // Queries can optionally include an OR clause which is
                // evaluated if WHERE doesn't match
                OR {
                    property: ""
                    value: "value 3"
                    matchRule: Query.Equals
                },
                // Queries can also have multiple AND clauses which
                // are evaluated if WHERE or OR find a match.
                AND {
                    property: ""
                    value: undefined
                    matchRule: Query.LessThan
                },
                AND {
                    // You can also provide your own custom comparitor for WHERE, OR, AND if
                    // you want to do something a little different or the default comparitor
                    // doesn't support your data type yet.
                    // The callback function will be passed the value to compare and you
                    // must return a bool result.
                    comparitor: function (value) {
                        console.log("Custom comparator callback! ", value)
                        return true;
                    }
                }
            ]
        }
    }
//    MainView {
//        anchors.fill: parent

//        Page {
//            id: page
//            title: "MazDB Example"



//            MazDBListModel {
//                id: mazDbListModel

//                /* Uncomment below to filter by a specific Key */
////                filter: "item1"
//                filterPolicy: MazDBListModel.Contains // there's also MazDBListModel.ExactMatch && MazDBListModel.StartsWith

//                // MazDBListModel _doesn't_ take a MazDB instance as it's source
//                // but rather the db's location for which the listmodel should observe.
//                // the model will listen for put/del events as well
//                // as do an initial load of any currently stored data.
//                source: dbSettings.source

//                /* By default this ListModel only has two rolenames "key" and "value" which
//                 * represent the key/value data stored in the leveldb instance.
//                 * You can extend these by adding customRoles which are appended to the roleNames() method.
//                 * These can then be accessed in your model delegates with model.${customRole}
//                 */
//                customRoles: ["hello", "world"]

//                /* If you supply custom roles then you need to implement a callback
//                 * that the list model will use to resolve a rolename.
//                 * The callback is passed the requested "role" and the current index's "key" and "value".
//                 * You can return any QVariant that can be handled by your delegate from this callback.
//                 */
//                rolesCallBack: function (role, key, value) {
//                    console.log("Custom roles callback called!")
//                    if (role === "hello") {
//                        return dbSettings.helloValue + value
//                    } else {
//                        return dbSettings.worldValue + role
//                    }
//                }
//            }

//            MazDBSortProxy {
//                id: proxy

//                // make sure the listmodel is initialized before adding it to the proxy
//                model: mazDbListModel.source ? mazDbListModel : null

//                // Which property you want to sort on
//                sortBy: "value"

//                // Sort order. Default is Qt.Ascending
//                sortOrder: Qt.DescendingOrder

//                /* You can supply a custom callback for the sort handling.
//                 * this will override calling QSortFilterProxyModel::lessThan()
//                 * From here you can sculpt your own ordering recipe. The passed arguments left/right
//                 * is the data behind the sortBy rolename of the two items being compared.
//                 * So using the "value" role and the returned raw variant object
//                 * you can actually come up with some pretty complex rules.
//                 *
//                 * And guess what... no need to dive into c++ for this anymore :-D
//                 */
//                sortCallBack: function (left, right) {
//                    console.log("Custom sort callback called!")
//                    return left < right
//                }
//            }

//            ListView {
//                anchors.fill: parent
//                model: proxy
//                delegate: ListItem {
//                    height: layout.height + divider.height
//                    ListItemLayout {
//                        id: layout
//                        title.text: model.value
//                        // let's call one of the customRoles and trigger the rolesCallBack
//                        subtitle.text: index % 2 ? model.world : model.hello
//                    }
//                    // delete item
//                    onClicked: db.del(model.key)
//                }
//            }
//        }
//    }
}
