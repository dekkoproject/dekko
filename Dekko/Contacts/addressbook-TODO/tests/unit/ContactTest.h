#ifndef CONTACTTEST_H
#define CONTACTTEST_H

#include <QObject>

class ContactTest : public QObject
{
    Q_OBJECT
public:
    explicit ContactTest(QObject *parent = 0);

private slots:
//    void testCreate();
};

#endif // CONTACTTEST_H
