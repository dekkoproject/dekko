import qbs
import qbs.Probes as Probes

Product {
    name: "QmfClient"
    targetName: "qmfclient5"
    type: "dynamiclibrary"

    Probes.PkgConfigProbe {
        id: accountsqt5
        name: "accounts-qt5"
    }

    Probes.PkgConfigProbe {
        id: signonqt5
        name: "libsignon-qt5"
    }

    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.network" }
    Depends { name: "Qt.sql" }

    property stringList pkgLibs: []
    cpp.linkerFlags: {
        if (!project.uoaEnabled) {
            return []
        }

        if (accountsqt5.found) {
            pkgLibs.concat(accountsqt5.libs)
        } else {
            throw "Missing accounts qt5 libs"
        }

        if (signonqt5.found) {
            pkgLibs.concat(signonqt5.libs)
        } else {
            throw "Missing signon qt5 libs"
        }
        return pkgLibs
    }

    property stringList pkgFlags: []
    cpp.cxxFlags: {

        if (!project.uoaEnabled) {
            return []
        }
        if (accountsqt5.found) {
            pkgFlags.concat(accountsqt5.cflags)
        } else {
            throw "Missing accounts qt5 cflags: " + accountsqt5.cflags
        }

        if (signonqt5.found) {
            pkgFlags.concat(signonqt5.cflags)
        } else {
            throw "Missing signon qt5 cflags"
        }
        return pkgFlags;
    }

    property stringList defaultDefines: [
        "QT_BUILD_QCOP_LIB",
        "QMF_INTERNAL",
        "QMF_INSTALL_ROOT=\"" + project.qmfInstallRoot + "\"",
        "SNAP",
        "QMF_ENABLE_LOGGING"
    ]
    cpp.defines: defaultDefines

        /*{
        if (project.snapMode) {
            defaultDefines.concat("SNAP")
        }
        if (project.unity8) {
            defaultDefines.concat("IS_UNITY8")
        }
        if (project.enableLogging) {
            defaultDefines.concat("QMF_ENABLE_LOGGING")
        }
        return defaultDefines
    }*/

    cpp.optimization: qbs.buildVariant === "debug" ? "none" : "fast"
    cpp.debugInformation: qbs.buildVariant === "debug"
    cpp.cxxLanguageVersion: "c++11";
    cpp.cxxStandardLibrary: "libstdc++";
    cpp.includePaths: ["libraries/qmfclient/", "libraries/qmfclient/support"]

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.gui" }
        Depends { name: "Qt.network" }
        Depends { name: "Qt.sql" }
        cpp.cxxLanguageVersion: "c++11";
        cpp.cxxStandardLibrary: "libstdc++";
        cpp.includePaths: ["libraries/qmfclient/", "libraries/qmfclient/support"]
    }

    Group {
        name: "C++ Sources"
        prefix: "libraries/qmfclient/"
        files: [
            "longstream.cpp",
            "longstring.cpp",
            "qmailaccount.cpp",
            "qmailaccountconfiguration.cpp",
            "qmailaccountkey.cpp",
            "qmailaccountlistmodel.cpp",
            "qmailaccountsortkey.cpp",
            "qmailaction.cpp",
            "qmailaddress.cpp",
            "qmailcodec.cpp",
            "qmailcontentmanager.cpp",
            "qmaildatacomparator.cpp",
            "qmaildisconnected.cpp",
            "qmailfolder.cpp",
            "qmailfolderfwd.cpp",
            "qmailfolderkey.cpp",
            "qmailfoldersortkey.cpp",
            "qmailid.cpp",
            "qmailinstantiations.cpp",
            "qmailkeyargument.cpp",
            "qmailmessage.cpp",
            "qmailmessagefwd.cpp",
            "qmailmessagekey.cpp",
            "qmailmessagelistmodel.cpp",
            "qmailmessagemodelbase.cpp",
            "qmailmessageremovalrecord.cpp",
            "qmailmessageserver.cpp",
            "qmailmessageset.cpp",
            "qmailmessagesortkey.cpp",
            "qmailmessagethreadedmodel.cpp",
            "qmailserviceaction.cpp",
            "qmailstore.cpp",
            "qmailstore_p.cpp",
            "qmailstoreimplementation_p.cpp",
            "qmailtimestamp.cpp",
            "qmailthread.cpp",
            "qmailthreadkey.cpp",
            "qmailthreadlistmodel.cpp",
            "qmailthreadsortkey.cpp",
            "qprivateimplementation.cpp",
            "locks.cpp"
        ]
    }

    Group {
        name: "C++ Header"
        prefix: "libraries/qmfclient/"
        files: [
            "*.h"
        ]
    }

    Group {
        name: "Support Sources"
        prefix: "libraries/qmfclient/support/"
        files: [
            "qmailnamespace.cpp",
            "qmaillog.cpp",
            "qlogsystem.cpp",
            "qloggers.cpp",
            "qcopadaptor.cpp",
            "qcopapplicationchannel.cpp",
            "qcopchannel.cpp",
            "qcopchannelmonitor.cpp",
            "qcopserver.cpp",
            "qmailpluginmanager.cpp"
        ]
    }
    Group {
        name: "Support Headers"
        prefix: "libraries/qmfclient/support/"
        files: [
            "*.h"
        ]
    }

    Group {
        condition: project.uoaEnabled
        name: "UOA Support"
        prefix: "libraries/qmfclient/uoa/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    Group {
        name: "Resources"
        prefix: "libraries/qmfclient/"
        files: [
            "qmf.qrc",
            "qmf_qt.qrc"
        ]

    }

    Group {
        name: "Translations"
        prefix: "libraries/qmfclient/"
        files: [
            "*.ts"
        ]
    }

    Group {
        qbs.install: true
        qbs.installDir: project.libDir
        fileTagsFilter: product.type
    }

    Group {
        qbs.install: true
        qbs.installDir: project.i18nDir
        fileTagsFilter: ["qm"]
    }
 }
