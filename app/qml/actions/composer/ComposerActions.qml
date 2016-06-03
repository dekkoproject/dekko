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
    signal addFileAttachment(string url)
    signal removeAttachment(int index)
    signal appendTextToBody(string text)
    // Add recipient from address book contacts
    // type: RecipientType enum value of To, Cc, Bcc
    // This is used by the contact picker to add to the correct model
    signal addRecipientFromContacts(int type)
    signal validateAddress(int type, string address)
    signal invalidAddress(int type, string address)
    signal validAddress(int type, string address)

    signal addRecipientIfValid(int type, string address)

    /* Add a recipient by email address

       type: RecipientType enum value
       address: valid email address
    */
    signal addRecipientFromAddress(int type, string address)

    /* Add a recipient by name & email address

       type: RecipientType enum value
       name: Recipients name
       address: valid email address

       This is useful when adding from a contacts list. The added recipient
       will be in the format "User name <user@email.com>"
    */
    signal addRecipientFromNameAddress(int type, string name, string address)

    signal removeRecipient(int type, int index)

    signal discardMessageConfirmed()

    /* Set which identity the MessageBuilder should use

       idx: index of the identity in SenderIdentities model
    */
    signal setIdentity(int idx)
}
