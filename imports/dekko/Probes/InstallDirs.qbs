import qbs
import qbs.Process
import qbs.Environment
import qbs.FileInfo

Probe {

    property bool overrideInstallPrefix: false
    property bool multiarch: true
    property string arch: "amd64"
    property string prefix: "usr"
    property string bindir: "bin"
    property string libdir: "lib"
    property string includedir: "include"
    property string datadir: "share"
    property string localedir: FileInfo.joinPaths(datadir, "locale")
    property string mandir: FileInfo.joinPaths(datadir, "man")
    property string docdir: FileInfo.joinPaths(datadir, "doc")

    configure: {
        if (multiarch) {
            // use process to query dpkg-architecture
            var p = new Process()
            try {
                if (p.exec("dpkg-architecture", ["-q", "DEB_HOST_MULTIARCH"], true) === 0) {
                    arch = p.readLine()
                } else {
                    throw "call to dpkg-architecture failed: \n" + p.readStdErr()
                }
            } finally {
                p.close()
            }
        }

        if (!qbs.installPrefix || overrideInstallPrefix) {

            var setPath = function(pre, path, multi, arch) {
                if (multi) {
                    return FileInfo.joinPaths(pre, path, arch)
                } else {
                    return FileInfo.joinPaths(pre, path)
                }
            }
            bindir = setPath(prefix, bindir, false, "")
            libdir = setPath(prefix, libdir, multiarch, arch)
            includedir = setPath(prefix, includedir, false, "")
            datadir = setPath(prefix, datadir, false, "")
            localedir = setPath(prefix, localedir, false, "")
            mandir = setPath(prefix, mandir, false, "")
            docdir = setPath(prefix, docdir, false, "")
        }
    }
}
