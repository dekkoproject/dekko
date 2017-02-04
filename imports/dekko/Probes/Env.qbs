import qbs
import qbs.Environment

Probe {
    id: env
    property string key: ""
    property string value: ""
    property bool valid: false
    configure: {
        value = Environment.getEnv(key)
        console.info("GetEnv: " + key + "=" +value)
        valid = typeof(value) !== undefined
    }
}
