/* Copyright (C) 2016 Dan Chapman <dpniel@ubuntu.com>

   This file is part of Dekko email client for Ubuntu devices

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.AutoConfig 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Composer 1.0

AppListener {
    waitFor: [ComposerStore.listenerId]

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
                ComposerStore.builder.addRecipient(MessageBuilder.To, message.address)
                break;
            case RecipientType.Cc:
                ComposerStore.builder.addRecipient(MessageBuilder.Cc, message.address)
                break;
            case RecipientType.Bcc:
                ComposerStore.builder.addRecipient(MessageBuilder.Bcc, message.address)
                break;
            }
        }
    }
    Filter {
        type: ComposerKeys.addRecipientFromNameAddress
        onDispatched: {
            switch(message.type) {
            case RecipientType.To:
                ComposerStore.builder.addRecipient(MessageBuilder.To, message.name, message.address)
                break;
            case RecipientType.Cc:
                ComposerStore.builder.addRecipient(MessageBuilder.Cc, message.name, message.address)
                break;
            case RecipientType.Bcc:
                ComposerStore.builder.addRecipient(MessageBuilder.Bcc, message.name, message.address)
                break;
            }
        }
    }

    Filter {
        type: ComposerKeys.removeRecipient
        onDispatched: {
            switch(message.type) {
            case RecipientType.To:
                ComposerStore.builder.removeRecipient(MessageBuilder.To, message.index)
                break;
            case RecipientType.Cc:
                ComposerStore.builder.removeRecipient(MessageBuilder.Cc, message.index)
                break;
            case RecipientType.Bcc:
                ComposerStore.builder.removeRecipient(MessageBuilder.Bcc, message.index)
                break;
            }
        }
    }

    Filter {
        type: ComposerKeys.setIdentity
        onDispatched: {
            if (message.idx < 0 || message.idx > ComposerStore.identitiesModel.count) {
                Log.logError("ComposerWorker::setIdentity", "Trying to set unknown index: %1".arg(message.idx))
                return
            }
            if (message.idx === ComposerStore.identityIndex) {
                Log.logInfo("ComposerWorker::setIdentity", "Identity at index already set as selected account")
                return
            }
            Log.logStatus("ComposerWorker::setIdentity", "Setting identity at index: %1".arg(message.idx))
            ComposerStore.identities.setSelectedIndex(message.idx)
        }
    }
    Filter {
        type: ComposerKeys.discardMessageConfirmed
        onDispatched: {
            Log.logInfo("ComposerWorker::discardMessage", "Discarding message")
            ComposerActions.resetComposer()
            ComposerStore.submission.discard()
            // We can just call close now as we don't need to wait on anything with resetComposer
            ViewActions.closeComposer()
        }
    }

    AppScript {
        readonly property string dlgId: "discard-confirmation-dialog"
        runWhen: ComposerKeys.discardMessage
        script: {
            if (ComposerStore.builder.hasDraft) {
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
            Log.logStatus("ComposerWorker::resetComposer", "Resetting composer ready for next message")
            ComposerStore._private.ccVisible = false
            ComposerStore._private.bccVisible = false
            ComposerStore.identities.reset()
            ComposerStore.submission.reset()
        }
    }
    Filter {
        type: ComposerKeys.addCC
        onDispatched: {
            Log.logInfo("ComposerWorker::addCC", "Displaying CC field")
            ComposerStore._private.ccVisible = true
        }
    }

    Filter {
        type: ComposerKeys.addBCC
        onDispatched: {
            Log.logInfo("ComposerWorker::addBCC", "Displaying BCC field")
            ComposerStore._private.bccVisible = true
        }
    }

    Filter {
        type: ComposerKeys.sendMessage
        onDispatched: {
            Log.logInfo("ComposerWorker::sendMessage", "Sending message...")
            ComposerStore._private.sendInProgress = true
            ComposerStore._private.sendTimer.start()
        }
    }

    Filter {
        type: ComposerKeys.respondToMessage
        onDispatched: {
            Log.logInfo("ComposerWorker::replyToMessage", "Constructing reply message for msg: " + message.msgId)
            ViewActions.openMessageComposer()
            ComposerStore.submission.respondToMessage(message.type, message.msgId)
        }
    }

    Filter {
        type: ComposerKeys.forwardMessage
        onDispatched: {
            Log.logInfo("ComposerWorker::forwardMessage", "Constructing message to forward")
            ViewActions.openMessageComposer()
            ComposerStore.submission.forwardMessage(message.type, message.msgId)
        }
    }

    Filter {
        type: ComposerKeys.saveDraft
        onDispatched: {
            Log.logInfo("ComposerWorker::saveDraft", "Saving draft...")
            ComposerStore.submission.saveDraft(true)
            ViewActions.closeComposer()
            ComposerActions.resetComposer()
        }
    }

    Filter {
        type: ComposerKeys.addFileAttachment
        onDispatched: {
            if (message.url.isEmpty()) {
                Log.logWarning("ComposerWorker::addFileAttachment", "Attachment url is empty")
                return;
            }
            Log.logInfo("ComposerWorker::addFileAttachment", "Adding %1 to attachments".arg(message.url))
            ComposerStore.builder.addFileAttachment(message.url)
        }
    }

    Filter {
        type: ComposerKeys.removeAttachment
        onDispatched: {
            Log.logInfo("ComposerWorker::removeAttachment", "Removing attachment at index: %1".arg(message.index))
            ComposerStore.builder.removeAttachment(message.index)
        }
    }

    Filter {
        type: ComposerKeys.addRecipientFromContacts
        onDispatched: {
            Log.logWarning("ComposerWorker::addRecipientFromContacts", "Not implemented yet")
        }
    }

    Filter {
        type: ComposerKeys.appendTextToBody
        onDispatched: {
            ComposerStore.builder.appendTextToBody(message.text)
        }
    }

    Filter {
        type: ComposerKeys.appendTextToSubject
        onDispatched: {
            ComposerStore.builder.appendTextToSubject(message.text)
        }
    }

    Filter {
        type: ComposerKeys.openDraft
        onDispatched: {
            Log.logInfo("ComposerWorker::openDraft", "Constinuing draft message")
            ViewActions.openMessageComposer()
            ComposerStore.submission.reloadDraft(message.draftId)
        }
    }

    Filter {
        type: ComposerKeys.composeMailtoUri
        onDispatched: {
            ComposerStore.builder.composeMailTo(message.mailto)
            // we need to delay this now as we may have actually only just been opened
            // and the various stores are still getting initialized as the UI is constructed
            ViewActions.delayCall(ViewKeys.openMessageComposer)
        }
    }
}
