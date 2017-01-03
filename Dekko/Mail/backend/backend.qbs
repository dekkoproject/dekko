import qbs

Project {
    name: "Backend"

    references: [
        "accounts/accounts.qbs",
        "accounts/quick/accountsplugin.qbs",
        "mail/mail.qbs",
        "mail/quick/mailplugin.qbs",
        "network/network.qbs",
        "settings/settings.qbs",
        "settings/quick/settingsplugin.qbs",
        "autoconfig/autoconfig.qbs"
    ]
}
