pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Accounts 1.0
import Dekko.AutoConfig 1.0
import Dekko.Mail 1.0
import "../../actions/composer"
import "../../actions/logging"
import "../../actions/views"
import "../../actions/popups"
import "../accounts"

AppListener {

    readonly property bool showCC: d.ccVisible
    readonly property bool showBCC: d.bccVisible
    readonly property bool sendInProgress: d.sendInProgress
    readonly property bool hasValidIdentity: d.identitiesValid
    readonly property QtObject identity: priv_identity
    readonly property QtObject recipients: priv_recipients

    property ComposerStoreActions actions: ComposerStoreActions{}

    property alias identitiesModel: identities.accountsModel
    property alias identityIndex: identities.selectedIndex
    // bind the subject fields text document to messagebuilder
    property alias subjectDocument: builder.subject
    // bind the body fields text document to messagebuilder
    property alias bodyDocument: builder.body

    SenderIdentities {
        id: identities
        accountsModel: AccountStore.sendAccountsModel
    }

    MessageBuilder {
        id: builder
        identities: identities
    }

    SubmissionManager {
        id: submissionManager
        builder: builder
        onError: {
            switch(error) {
                case SubmissionManager.NoBuilder:
                Log.logError("ComposerStore::SubmissionManager::NoBuilder", "No message builder set. Cannot compose without one")
                break
                case SubmissionManager.InvalidMessage:
                Log.logError("ComposerStore::SubmissionManager::InvalidMessage", "Invalid message, missing 'To' recipients or Subject")
                break
                case SubmissionManager.NoIdentities:
                Log.logError("ComposerStore::SubmissionManager::NoIdentities", "No identities available for MessageBuilder. Cannot proceed...")
                break
            }
        }
        onMessageQueued: {
            Log.logInfo("ComposerStore::SubmissionManager::messageQueued", "Message queued resetting composer")
            ComposerActions.resetComposer()
            ViewActions.closeComposer()
            d.sendInProgress = false
        }
        onDraftSaved: {
            Log.logInfo("ComposerStore::SubmissionManager::draftSaved", "Draft message was saved");
            ViewActions.orderSimpleToast(qsTr("Draft saved."))
        }
    }

    QtObject {
        id: priv_identity
        readonly property string name: hasValidIdentity ? identities.selectedAccount.outgoing.name : ""
        readonly property string initials: hasValidIdentity ? identities.selectedAccount.outgoing.initials : ""
        readonly property string email: hasValidIdentity ? identities.selectedAccount.outgoing.email : ""
    }

    QtObject {
        id: priv_recipients
        property alias to: builder.to
        property alias cc: builder.cc
        property alias bcc: builder.bcc
    }

    QtObject {
        id: d
        property bool sendInProgress: false
        property bool ccVisible: false
        property bool bccVisible: false
        property bool identitiesValid: identities.selectedIndex >= 0
        property Timer delayDiscard: Timer {
            interval: 10
            onTriggered: ComposerActions.discardMessage()
        }
        property Timer sendTimer: Timer {
            interval: 50
            onTriggered: submissionManager.send()
        }
    }

    Filter {
        type: ComposerKeys.validateAddress
        onDispatched: {
            if (EmailValidator.validate(message.address)) {
                ComposerActions.validAddress(message.type, message.address)
            } else {
                ComposerActions.invalidAddress(message.type, message.address)
            }
        }
    }

    Filter {
        type: ComposerKeys.addRecipientFromAddress
        onDispatched: {
            switch(message.type) {
                case RecipientType.To:
                builder.addRecipient(MessageBuilder.To, message.address)
                break;
                case RecipientType.Cc:
                builder.addRecipient(MessageBuilder.Cc, message.address)
                break;
                case RecipientType.Bcc:
                builder.addRecipient(MessageBuilder.Bcc, message.address)
                break;
            }
        }
    }

    Filter {
        type: ComposerKeys.addRecipientFromNameAddress
        onDispatched: {
            switch(message.type) {
                case RecipientType.To:
                builder.addRecipient(MessageBuilder.To, message.name, message.address)
                break;
                case RecipientType.Cc:
                builder.addRecipient(MessageBuilder.Cc, message.name, message.address)
                break;
                case RecipientType.Bcc:
                builder.addRecipient(MessageBuilder.Bcc, message.name, message.address)
                break;
            }
        }
    }

    Filter {
        type: ComposerKeys.removeRecipient
        onDispatched: {
            switch(message.type) {
                case RecipientType.To:
                builder.removeRecipient(MessageBuilder.To, message.index)
                break;
                case RecipientType.Cc:
                builder.removeRecipient(MessageBuilder.Cc, message.index)
                break;
                case RecipientType.Bcc:
                builder.removeRecipient(MessageBuilder.Bcc, message.index)
                break;
            }
        }
    }

    Filter {
        type: ComposerKeys.setIdentity
        onDispatched: {
            if (message.idx < 0 || message.idx > identities.accountsModel.count) {
                Log.logError("ComposerStore::setIdentity", "Trying to set unknown index: %1".arg(message.idx))
                return
            }
            if (message.idx === identities.selectedIndex) {
                Log.logInfo("ComposerStore::setIdentity", "Identity at index already set as selected account")
                return
            }
            Log.logStatus("ComposerStore::setIdentity", "Setting identity at index: %1".arg(message.idx))
            identities.setSelectedIndex(message.idx)
        }
    }

    Filter {
        type: ComposerKeys.discardMessageConfirmed
        onDispatched: {
            Log.logInfo("ComposerStore::discardMessage", "Discarding message")
            ComposerActions.resetComposer()
            submissionManager.discard()
            // We can just call close now as we don't need to wait on anything with resetComposer
            ViewActions.closeComposer()
        }
    }

    AppScript {
        readonly property string dlgId: "discard-confirmation-dialog"
        runWhen: ComposerKeys.discardMessage
        script: {
            if (builder.hasDraft) {
                PopupActions.showConfirmationDialog(dlgId, qsTr("Discard message"), qsTr("Are you sure you want to discard this message?"))
            } else {
                ComposerActions.discardMessageConfirmed()
                exit.bind(this,0)
                return;
            }

            once(PopupKeys.confirmationDialogConfirmed, function(message){
                if (message.id === dlgId) {
                    ComposerActions.discardMessageConfirmed()
                }
            })
            // If for some wild reason the composer get's reset while the confirmation
            // dialog is up just cancel all bindings.
            once(ComposerKeys.resetComposer, exit.bind(this, 0))
        }
    }

    Filter {
        type: ComposerKeys.resetComposer
        onDispatched: {
            Log.logStatus("ComposerStore::resetComposer", "Resetting composer ready for next message")
            d.ccVisible = false
            d.bccVisible = false
            identities.reset()
            submissionManager.reset()
        }
    }

    Filter {
        type: ComposerKeys.addCC
        onDispatched: {
            Log.logInfo("ComposerStore::addCC", "Displaying CC field")
            d.ccVisible = true
        }
    }

    Filter {
        type: ComposerKeys.addBCC
        onDispatched: {
            Log.logInfo("ComposerStore::addBCC", "Displaying BCC field")
            d.bccVisible = true
        }
    }

    Filter {
        type: ComposerKeys.sendMessage
        onDispatched: {
            Log.logInfo("ComposerStore::sendMessage", "Sending message...")
            d.sendInProgress = true
            d.sendTimer.start()
        }
    }

    Filter {
        type: ComposerKeys.saveDraft
        onDispatched: {
            Log.logInfo("ComposerStore::saveDraft", "Saving draft...")
            submissionManager.saveDraft(true)
            ViewActions.closeComposer()
            ComposerActions.resetComposer()
        }
    }

    Filter {
        type: ComposerKeys.addAttachment
        onDispatched: {
            Log.logWarning("ComposerStore::addAttachment", "Not implemented yet")
        }
    }

    Filter {
        type: ComposerKeys.removeAttachment
        onDispatched: {
            Log.logWarning("ComposerStore::removeAttachment", "Not implemented yet")
        }
    }

    Filter {
        type: ComposerKeys.addRecipientFromContacts
        onDispatched: {
            Log.logWarning("ComposerStore::addRecipientFromContacts", "Not implemented yet")
        }
    }
}

