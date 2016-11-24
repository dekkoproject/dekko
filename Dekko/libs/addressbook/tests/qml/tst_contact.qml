import QtQuick 2.4
import QtQuick.Window 2.1
import QtTest 1.0
import Dekko.Contacts 1.0

Item {
    id: test
    width: 640
    height: 480

    Contact {
        id: contact
    }

    Component {
        id: newEmail
        Email {
            type: DetailBase.String
        }
    }

    TestCase {
        name: "Contact"

        function test_contact_object() {
            contact.fullName.type = DetailBase.String
            contact.fullName.value = "Dan Chapman"
            compare(contact.toVariant().fn.value, "Dan Chapman")
            var email = newEmail.createObject()
            email.value = "dpniel@ubuntu.com"
            contact.emailList.append(email)
            compare(contact.emailList.count, 1)
            compare(contact.toVariant().email[0].type, "text")
            compare(contact.toVariant().email[0].value, "dpniel@ubuntu.com")
        }
    }
}
