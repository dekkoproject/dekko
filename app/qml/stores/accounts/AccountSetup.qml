pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Accounts 1.0
import Dekko.AutoConfig 1.0
import "../../actions/wizard"
import "../../actions/logging"
import "../../views/utils/QtCoreAPI.js" as QtCoreAPI

AppListener {
    id: accountSetup

    // We emit these signals for the DSM.SignalTransition's
    signal createAccount()
    signal goBack()
    signal goNext()
    signal userDetailsSet()

    // new account accessor for the setupwizard
    property alias account: account

    // Properties regarding new account type
    readonly property alias isPreset: d.isPresetAccount
    readonly property alias isImap: d.isImapAccount
    readonly property alias isPop3: d.isPopAccount
    readonly property alias accountDescripion: d.accountDescripion

    // New account object
    // This is a subclass of Account that allows editing of
    // an invalid account before comitting to the QMailStore
    NewAccount {
        id: account
    }

    Filter {
        type: WizardKeys.setNewAccountType
        onDispatched: {
            d.accountDescripion = message.description
            var type = message.type
            switch (message.type) {
            case AccountConfig.IMAP:
                d.isImapAccount = true;
                break;
            case AccountConfig.POP3:
                d.isPopAccount = true;
                break;
            case AccountConfig.SMTP:
                type = AccountConfig.IMAP
                break;
            }
            account.setSourceType(type)
        }
    }
    Filter {
        type: WizardKeys.setUserDetails
        onDispatched: {
            var details = message.details
            // Name is username for imap/pop
            account.incoming.name = details.email
            account.incoming.email = details.email
            account.incoming.password = details.password
            account.outgoing.name = details.name
            account.outgoing.email = details.email
            account.outgoing.username = details.email
            account.outgoing.password = details.password
            Log.logInfo("AccountSetup::setUserDetails: ", "name: %1, email: %2".arg(details.name).arg(details.email))
            accountSetup.userDetailsSet()
        }
    }

    Filter {
        type: WizardKeys.validateUser
        onDispatched: {
            var user = message.user
            var valid = true
            var invalidField = new Array();
            if (user.name.isEmpty()) {
                valid = false
                invalidField.push("name")
            }
            if (!EmailValidator.validate(user.email)) {
                valid = false
                invalidField.push("email")
            }
            if (!valid) {
                //we want to signal this first as the user may want an empty password field
                WizardActions.userDetailsInvalid(invalidField)
            } else if (!d.allowEmptyPassword && user.password.isEmpty()) {
            // if the password is empty and the user hasn't confirmed it then
            // We have to prompt for them to allow it.
                WizardActions.noPasswordSet()
            } else {
                WizardActions.userDetailsValid()
            }
        }
    }

    Filter {
        type: WizardKeys.setNoPasswordAllowed
        onDispatched: {
            d.allowEmptyPassword = true
        }
    }

    Filter {
        type: WizardKeys.wizardStepBack
        onDispatched: {
            // We don't actually want to do anything here
            // as we need to be sure the wizardStepBack listener on the StackView
            // pops the page before emitting goBack, otherwise
            // we end up with a push instead of pop
        }
    }
    Filter {
        type: WizardKeys.wizardStepForward
        onDispatched: {
            accountSetup.goNext()
        }
    }

    Filter {
        type: WizardKeys.wizardResetAccount
        onDispatched: {
            // TODO: reset account
            Log.logError("AccountSetup::resetAccount", "FIXME: implement account reset")
            d.isPresetAccount = false
            d.isImapAccount = false
            d.isPopAccount = false
            d.accountDescripion = ""
            d.allowEmptyPassword = false
        }
    }

    Filter {
        type: WizardKeys.wizardSetAccountPreset
        onDispatched: {
            var cfg = message.config
            if (cfg.type !== Provider.PRESET) {
                Log.logError("AccountSetup::wizardSetPreset", "Passed config not of type Provider.PRESET. Aborting...")
                return
            }
            Log.logInfo("AccountSetup::wizardSetPreset", "ImapHost: %1, Port: %2".arg(cfg.imapHost).arg(cfg.imapPort))
            account.incoming.server = cfg.imapHost
            account.incoming.port = cfg.imapPort
            account.incoming.encryption = d.getPresetEncryptionMethod(cfg.imapStartTLS, cfg.imapUseSSL)
            account.incoming.saslMechanism = AccountConfig.LOGIN
            account.outgoing.server = cfg.smtpHost
            account.outgoing.port = cfg.smtpPort
            account.outgoing.encryption = d.getPresetEncryptionMethod(cfg.smtpStartTLS, cfg.smtpUseSSL)
            account.outgoing.saslMechanism = AccountConfig.LOGIN
            d.isPresetAccount = true
        }
    }
    // We don't expose any utility functions or properties, as developers
    // should dispatch actions via the actions api's.
    QtObject {
        id: d
        property bool isImapAccount: false
        property bool isPopAccount: false
        property bool isPresetAccount: false
        property string accountDescripion: ""
        property bool allowEmptyPassword: false

        function getPresetEncryptionMethod(useStartTls, useSsl) {
            if (useSsl && useStartTls) {
                return AccountConfig.SSL;
            } else if (useStartTls) {
                return AccountConfig.STARTTLS;
            } else {
                return AccountConfig.NONE;
            }
        }
    }
}


