# MazDB

MazDB is a Qt/QML NoSQL database and utilities library. It is built ontop 
of Google's leveldb, and loosely based on the work done in https://github.com/paulovap/qtleveldb 

The main components to MazDB are:

* `MazDB` - This is the low level api to the underlying leveldb
* `MazDBListModel` - This is a properly behaving qt model with some nice extras for manipulating your unstructured data
* `MazDBSortProxy` - Designed to work with `MazDBListModel` and providing sorting of your unstructured data.
* `MazDBSettings` - a QSettings/qt.labs.Settings replacement
* `MazDBQueryEngine` - A low level query engine for querying MazDB.
* `MazDBQuery` - Query component to be used with MazDBQueryEngine usinga declarative query language (MazQL)


## Build

Then fetch maz-db source

```bash
$ git clone http://code.dekkoproject.org/dekko-dev/maz-db.git
$ cd maz-db
$ qbs <options>
```

To run the example app

`$ qbs run`

To run the tests

`$ qbs build -p autotest-runner`


# Usage (Still TODO)

For now take a look at the test suite and the [example/qml/Main.qml](http://code.dekkoproject.org/dekko-dev/maz-db/blob/master/example/qml/Main.qml) file
