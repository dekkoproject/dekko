pragma Singleton
import QtQuick 2.4
import QuickFlux 1.0

ActionCreator {

    signal resetComposer()
    signal addCC()
    signal addBCC()
    signal sendMessage()
    signal saveDraft()
    signal discardMessage()
    signal addAttachment()
    signal removeAttachment()
    // Add recipient from address book contacts
    // type: RecipientType enum value of To, Cc, Bcc
    // This is used by the contact picker to add to the correct model
    signal addRecipientFromContacts(int type)

    signal setIdentity(int idx)
}
