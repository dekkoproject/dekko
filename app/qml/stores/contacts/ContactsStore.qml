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
import Dekko.Contacts 1.0
import "../../actions/contacts"
import "../../actions/logging"

AppListener {

    readonly property var addressbooks: abooks.model ? abooks.model : 0
    property alias abooks: abooks
    readonly property int selectedAddressBookIndex: d.selectedIndex
    property AddressBook selectedAddressBook
    property alias combinedModel: combinedModel

    AddressBooks {
        id: abooks
        onPluginsLoaded: {
            ContactActions.setSelectedAddressBook(0);
        }
    }

    CombinedAddressBookModel {
        id: combinedModel
        addressBooks: abooks
    }

    Filter {
        type: ContactKeys.setSelectedAddressBook
        onDispatched: {
            Log.logInfo("ContactsStore::setSelectedAbook", "Switching addressbook")
            d.selectedIndex = message.idx
            selectedAddressBook = abooks.model.get(d.selectedIndex)
        }
    }

    Filter {
        type: ContactKeys.saveContact
        onDispatched: {
            Log.logInfo("ContactsStore::saveContact", "Saving contact details")
            abooks.saveContact(message.contact)
        }
    }

    Filter {
        type: ContactKeys.removeContact
        onDispatched: {
            Log.logInfo("ContactsStore::remvoeContact", "Removing contact")
            abooks.removeContact(message.contact)
        }
    }

    QtObject {
        id: d
        property int selectedIndex: 0
    }
}
