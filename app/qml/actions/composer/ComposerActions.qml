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
