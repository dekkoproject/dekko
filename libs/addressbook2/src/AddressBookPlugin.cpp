#include "AddressBookPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <AddressBooks.h>
#include <AddressBook.h>
#include <Collection.h>
#include <Contact.h>

void AddressBookPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Contacts"));

    // @uri Dekko.Contacts
    qmlRegisterType<AddressBooks>(uri, 1, 0, "AddressBooks");;

    qmlRegisterUncreatableType<AddressBook>(uri, 1, 0, "AddressBook", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Contact>(uri, 1, 0, "Contact");
    qmlRegisterUncreatableType<AddressBook>(uri, 1, 0, "AddressBook", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Collection>(uri, 1, 0, "Collection", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<DetailBase>(uri, 1, 0, "DetailBase", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Version>(uri, 1, 0, "Version", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<FullName>(uri, 1, 0, "FullName", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Name>(uri, 1, 0, "Name", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Birthday>(uri, 1, 0, "Birthday", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Anniversary>(uri, 1, 0, "Anniversary", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<Gender>(uri, 1, 0, "Gender", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Organization>(uri, 1, 0, "Organization");
    qmlRegisterUncreatableType<OrganizationList>(uri, 1, 0, "OrganizationList", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Address>(uri, 1, 0, "Address");
    qmlRegisterUncreatableType<AddressList>(uri, 1, 0, "AddressList", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Telephone>(uri, 1, 0, "Telephone" );
    qmlRegisterUncreatableType<TelephoneList>(uri, 1, 0, "TelephoneList", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Email>(uri, 1, 0, "Email");
    qmlRegisterUncreatableType<EmailList>(uri, 1, 0, "EmailList", QStringLiteral("Not creatable from qml"));
    qmlRegisterUncreatableType<TimeZone>(uri, 1, 0, "TimeZone", QStringLiteral("Not creatable from qml"));
    qmlRegisterType<Url>(uri, 1, 0, "Url");
    qmlRegisterUncreatableType<UrlList>(uri, 1, 0, "UrlList", QStringLiteral("Not creatable from qml"));
}

void AddressBookPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
