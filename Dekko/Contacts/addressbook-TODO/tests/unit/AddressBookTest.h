#ifndef ADDRESSBOOKTEST_H
#define ADDRESSBOOKTEST_H

#include <QObject>

class AddressBookTest : public QObject
{
    Q_OBJECT
public:
    explicit AddressBookTest(QObject *parent = 0);

private slots:
    void testCreate();
//    void testAddContact_data();
//    void testAddContact();
//    void testRemoveContact();
};

#endif // ADDRESSBOOKTEST_H
