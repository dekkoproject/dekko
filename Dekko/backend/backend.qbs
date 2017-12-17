import qbs

Project {
    name: "Backend"

    references: [
        "accounts/accounts.qbs",
        "accounts/quick/accountsplugin.qbs",
//        "accounts/service/service.qbs",
        "mail/mail.qbs",
        "mail/quick/mailplugin.qbs",
//        "mail/service/service.qbs",
        "network/network.qbs",
        "settings/settings.qbs",
        "settings/quick/settingsplugin.qbs",
        "autoconfig/autoconfig.qbs"
    ]
}
