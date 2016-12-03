import qbs
import "accounts/accounts.qbs" as AccountsLib
import "accounts/quick/accountsplugin.qbs" as AccountsPlugin
import "mail/mail.qbs" as MailLib
import "mail/quick/mailplugin.qbs" as MailPlugin
import "network/network.qbs" as NetworkLib
import "settings/settings.qbs" as SettingsLib
import "settings/quick/settingsplugin.qbs" as SettingsPlugin
import "autoconfig/autoconfig.qbs" as AutoConfigPlugin

Project {
    name: "Backend"

    AutoConfigPlugin {
    }

    AccountsLib {}
    AccountsPlugin {}

    SettingsLib {
    }

    SettingsPlugin {
    }

    MailLib {
    }

    MailPlugin {
    }

    NetworkLib {
    }
}
