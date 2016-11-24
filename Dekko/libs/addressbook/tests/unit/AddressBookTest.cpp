#include "AddressBookTest.h"
#include <QTest>
#include <QJsonObject>
#include <AddressBook.h>
#include <QmlObjectListModel.h>
#include <Contact.h>
#include "helpers.h"


AddressBookTest::AddressBookTest(QObject *parent) : QObject(parent)
{
}

void AddressBookTest::testCreate()
{
    auto abook = new AddressBook(this, Helpers::abookConfig());
    QVERIFY(abook != Q_NULLPTR);
    QCOMPARE(abook->name(), QStringLiteral(VALID_NAME));
    QCOMPARE(abook->canRead(), true);
    QCOMPARE(abook->canWrite(), false);
    QCOMPARE(abook->canDelete(), true);
    abook->deleteLater();
}

//void AddressBookTest::testAddContact_data()
//{
//    QTest::addColumn<QJsonObject>("contact");
//    QTest::addColumn<bool>("result");

//    QTest::newRow("invalid json object") << QJsonObject() << true;
//    QTest::newRow("valid contact") << Helpers::createContact("contact1") << false;
//}

//void AddressBookTest::testAddContact()
//{
//    auto abook = new AddressBook(this, Helpers::abookConfig());
//    QVERIFY(abook != Q_NULLPTR);

//    QFETCH(QJsonObject, contact);
//    QFETCH(bool, result);

//    abook->addContact(contact);

//    QCOMPARE(abook->isEmpty(), result);

//}

//void AddressBookTest::testRemoveContact()
//{
//    auto abook = new AddressBook(this, Helpers::abookConfig());
//    QVERIFY(abook != Q_NULLPTR);

//    abook->addContact(Helpers::createContact("contact1"));

//    QVERIFY(!abook->isEmpty());

//    abook->removeContact("contact1");

//    QVERIFY(abook->isEmpty());
//}

