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
pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0
import Dekko.Mail.Accounts 1.0
import Dekko.Mail 1.0
import Dekko.Mail.API 1.0
import Dekko.Mail.Stores.Accounts 1.0
import Dekko.Utils 1.0 as QtCoreAPI

AppListener {

    readonly property bool showCC: d.ccVisible
    readonly property bool showBCC: d.bccVisible
    readonly property bool sendInProgress: d.sendInProgress
    readonly property bool hasValidIdentity: d.identitiesValid
    readonly property bool sidePanelOpen: builder.attachments.count || d.openContacts
    readonly property string currentSignature: identities.selectedAccount.outgoing.signature
    readonly property QtObject identity: priv_identity
    readonly property QtObject recipients: priv_recipients
    readonly property QtObject attachments: builder.attachments
    readonly property QtObject __builder: builder

    property alias _private: d

    property ComposerStoreActions actions: ComposerStoreActions{}
    /////////////
    // Builder //
    /////////////
    property alias builder: builder
    // bind the subject fields text document to messagebuilder
    property alias subjectDocument: builder.subject
    // bind the body fields text document to messagebuilder
    property alias bodyDocument: builder.body
    ////////////////
    // identities //
    ////////////////
    property alias identities: identities
    property alias identitiesModel: identities.accountsModel
    property alias identityIndex: identities.selectedIndex
    ////////////////
    // Submission //
    ////////////////
    property alias submission: submissionManager

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
            ViewActions.orderSimpleToast(qsTr("Message queued."))
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
        property bool openContacts: false
        property Timer delayDiscard: Timer {
            interval: 10
            onTriggered: ComposerActions.discardMessage()
        }
        property Timer sendTimer: Timer {
            interval: 50
            onTriggered: submissionManager.send()
        }
    }
}

