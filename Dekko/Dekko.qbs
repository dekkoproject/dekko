import qbs
import "components/components.qbs" as DekkoComponents
import "utils/utils.qbs" as DekkoUtils

Project {
    name: "Dekko"

    property string appName: "Dekko"
    property string pkgName: "dekko.dekkoproject"
    property string version: "0.9.5"


    DekkoComponents {
        name: "Components"
    }

    DekkoUtils {
        name: "Shared Utils"
    }
    
    SubProject {
        filePath: "plugins/default-plugins.qbs"
        inheritProperties: true
    }
    
    SubProject {
        filePath: "Mail/mail.qbs"
        inheritProperties: true
    }
    
    SubProject {
        filePath: "Calendar/calendar.qbs"
        inheritProperties: true
    }
    
    SubProject {
        filePath: "Contacts/contacts.qbs"
        inheritProperties: true
    }
    
    SubProject {
        filePath: "Notes/notes.qbs"
        inheritProperties: true
    }
}
