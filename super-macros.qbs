import qbs

Project {
    name: "Super Macros"
    
    DynamicLibrary {
        name: "SuperMacros"
        cpp.includePaths: ["./"]
        
        Export {
            cpp.includePaths: ["./"]
        }
    }
}
