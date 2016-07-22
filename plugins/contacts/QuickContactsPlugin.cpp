#include "QuickContactsPlugin.h"
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <AddressBook.h>
#include <AddressBooks.h>
#include <Contact.h>
#include <ContactsPlugin.h>
#include <CombinedAddressBookModel.h>
#include <SortedContactsModel.h>
#include <SearchFilter.h>

void QuickContactsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("Dekko.Contacts"));
    // @uri Dekko.Contacts
    qmlRegisterType<AddressBook>(uri, 1, 0, "AddressBook");
    qmlRegisterType<AddressBooks>(uri, 1, 0, "AddressBooks");
    qmlRegisterType<Contact>(uri, 1, 0, "Contact");
    qmlRegisterType<ContactAddress>(uri, 1, 0, "ContactAddress");
    qmlRegisterType<ContactEmail>(uri, 1, 0, "ContactEmail");
    qmlRegisterType<ContactPhone>(uri, 1, 0, "ContactPhone");
    qmlRegisterType<CombinedAddressBookModel>(uri, 1, 0, "CombinedAddressBookModel");
    qmlRegisterType<SortedContactsModel>(uri, 1, 0, "SortedContactsModel");
    qmlRegisterType<SearchFilter>(uri, 1, 0, "SearchFilter");
}

void QuickContactsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
