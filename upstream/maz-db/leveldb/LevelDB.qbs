import qbs

DynamicLibrary {
    id: leveldb

    readonly property var includePaths: ["./leveldb", "./leveldb/include"]
    property string basePath: ""

    name: "LevelDB"
    targetName: "leveldb"
    Depends { name: "Qt.core" }
    Depends { name: "cpp" }

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.includePaths: leveldb.includePaths


    Properties {
        condition: project.useSnappy
        cpp.defines: [
            "LEVELDB_PLATFORM_QT",
            "SNAPPY"
        ]
        cpp.dynamicLibraries: ["snappy"]
    }

    Properties {
        condition: !project.useSnappy
        cpp.defines: [
            "LEVELDB_PLATFORM_QT"
        ]
    }

    cpp.cxxFlags: [
        "-fno-builtin-memcmp",
        "-pthread"
    ]

    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++"; // avoid issues with libc++

    Export {
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++"; // avoid issues with libc++
        cpp.includePaths: leveldb.includePaths

        Depends { name: "cpp"; }
        Depends { name: "Qt.core"; }
    }

    Group {
        name: "C++ Headers"
        prefix: path + "/include/leveldb/"
        files: [
            "c.h",
            "cache.h",
            "comparator.h",
            "db.h",
            "dumpfile.h",
            "env.h",
            "filter_policy.h",
            "iterator.h",
            "options.h",
            "slice.h",
            "status.h",
            "table.h",
            "table_builder.h",
            "write_batch.h"
        ]
    }

    Group {
        name: "C++ Sources"
        prefix: path + "/"
        files: [
            "db/builder.cc",
            "db/c.cc",
            "db/dbformat.cc",
            "db/db_impl.cc",
            "db/db_iter.cc",
            "db/filename.cc",
            "db/log_reader.cc",
            "db/log_writer.cc",
            "db/memtable.cc",
            "db/repair.cc",
            "db/table_cache.cc",
            "db/version_edit.cc",
            "db/version_set.cc",
            "db/write_batch.cc",
            "table/block_builder.cc",
            "table/block.cc",
            "table/filter_block.cc",
            "table/format.cc",
            "table/iterator.cc",
            "table/merger.cc",
            "table/table_builder.cc",
            "table/table.cc",
            "table/two_level_iterator.cc",
            "helpers/memenv/memenv.cc",
            "util/arena.cc",
            "util/bloom.cc",
            "util/cache.cc",
            "util/coding.cc",
            "util/comparator.cc",
            "util/crc32c.cc",
            "util/env.cc",
            "util/env_qt.cc",
            "util/filter_policy.cc",
            "util/hash.cc",
            "util/histogram.cc",
            "util/logging.cc",
            "util/options.cc",
            "util/status.cc"
        ]
    }
}
