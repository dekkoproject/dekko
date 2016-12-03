#include "ContactTest.h"
#include <QTest>
#include <Contact.h>
#include "helpers.h"

ContactTest::ContactTest(QObject *parent) : QObject(parent)
{
}

//void ContactTest::testCreate()
//{
//    Contact *contact  = Contact::fromJson(Helpers::createContact("contact1"));
//    QVERIFY(!contact->uid().isEmpty());
//    QCOMPARE(contact->name()->surname(), QStringLiteral("Man"));
//    QCOMPARE(contact->name()->forenames()->count(), 2);
//    QCOMPARE(contact->name()->forenames()->get(0).toString(), QStringLiteral("Dan"));
//}

