import qbs
import "./libraries/QmfClient.qbs" as QmfClient
import "./libraries/QmfServer.qbs" as QmfServer
import "./plugins/contentmanagers/StorageManager.qbs" as StorageManager
import "./plugins/messageservices/imap.qbs" as ImapServicePlugin
import "./plugins/messageservices/pop.qbs" as PopServicePlugin
import "./plugins/messageservices/smtp.qbs" as SmtpServicePlugin
import "./plugins/messageservices/settings.qbs" as SettingsServicePlugin

Project {
    name: "Messaging Framework"

    property string libDir: "lib"
    property string i18nDir: "lib/i18n"
    property string qmfInstallRoot: "lib/qmf"
    property bool snapMode: true
    property bool unity8: false
    property bool enableLogging: false
    // Rreadonly until fully functional again.
    readonly property bool uoaEnabled: false

    // Client library libqmfclient5
    QmfClient {}
    // Server library libqmfmessageserver5
    QmfServer {}
    // Storage ContentManager plugin
    StorageManager {}

    ImapServicePlugin {}

    PopServicePlugin {}

    SmtpServicePlugin {}

    SettingsServicePlugin {}
}
