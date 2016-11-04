#include <QGuiApplication>
#include <QtQuick>
#include <Mace.h>
#include <AddressBooks.h>
#include <AddressBook.h>
#include <Collection.h>
#include <Contact.h>

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<AddressBooks>("Dekko.Contacts", 0, 1, "AddressBooks");
    qmlRegisterUncreatableType<AddressBook>("Dekko.Contacts", 0, 1, "AddressBook", "Cannot be created in qml");
    qmlRegisterType<Contact>("Dekko.Contacts", 1, 0, "Contact");
    qmlRegisterUncreatableType<AddressBook>("Dekko.Contacts", 1, 0, "AddressBook", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Collection>("Dekko.Contacts", 1, 0, "Collection", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<DetailBase>("Dekko.Contacts", 1, 0, "DetailBase", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Version>("Dekko.Contacts", 1, 0, "Version", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<FullName>("Dekko.Contacts", 1, 0, "FullName", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Name>("Dekko.Contacts", 1, 0, "Name", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Birthday>("Dekko.Contacts", 1, 0, "Birthday", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Anniversary>("Dekko.Contacts", 1, 0, "Anniversary", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Gender>("Dekko.Contacts", 1, 0, "Gender", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Organization>("Dekko.Contacts", 1, 0, "Organization");
    qmlRegisterUncreatableType<OrganizationList>("Dekko.Contacts", 1, 0, "OrganizationList", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Address>("Dekko.Contacts", 1, 0, "Address");
    qmlRegisterUncreatableType<AddressList>("Dekko.Contacts", 1, 0, "AddressList", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Telephone>("Dekko.Contacts", 1, 0, "Telephone" );
    qmlRegisterUncreatableType<TelephoneList>("Dekko.Contacts", 1, 0, "TelephoneList", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Email>("Dekko.Contacts", 1, 0, "Email");
    qmlRegisterUncreatableType<EmailList>("Dekko.Contacts", 1, 0, "EmailList", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<TimeZone>("Dekko.Contacts", 1, 0, "TimeZone", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Url>("Dekko.Contacts", 1, 0, "Url");
    qmlRegisterUncreatableType<UrlList>("Dekko.Contacts", 1, 0, "UrlList", QStringLiteral("Not creatable from qml"));

    Mace::Runner runner;
    runner.include(ADDRESSBOOK_QML_TESTS);
//    runner.include<AddressBookTest>();
//    runner.include<ContactTest>();
    return runner.run(app.arguments());
}
