import qbs

Project {
    name: "Super Macros"
    
    DynamicLibrary {
        name: "SuperMacros"
        Depends { name: "cpp" }
        cpp.includePaths: ["./"]

        Group {
            name: "Macro Headers"
            files: ["*.h"]
            qbs.install: false
        }
        
        Export {
            cpp.includePaths: ["./"]
        }
    }
}
