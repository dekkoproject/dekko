#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <QQmlAutoPropertyHelpers.h>

typedef QString ContactId;
typedef QStringList ContactIdList;

class ContactEmail : public QObject
{
    Q_OBJECT
    QML_WRITABLE_AUTO_PROPERTY(QString, address)
public:
    explicit ContactEmail(QObject *parent = 0): QObject(parent) {}
};

class ContactAddress : public QObject
{
    Q_OBJECT
    QML_WRITABLE_AUTO_PROPERTY(QString, street)
    QML_WRITABLE_AUTO_PROPERTY(QString, street2)
    QML_WRITABLE_AUTO_PROPERTY(QString, city)
    QML_WRITABLE_AUTO_PROPERTY(QString, state)
    QML_WRITABLE_AUTO_PROPERTY(QString, country)
    QML_WRITABLE_AUTO_PROPERTY(QString, zip)
public:
    explicit ContactAddress(QObject *parent = 0): QObject(parent) {}
};

class ContactPhone : public QObject
{
    Q_OBJECT
public:
    explicit ContactPhone(QObject *parent = 0): QObject(parent) {}
    enum Type { Home, Work, Fax, Mobile };

    Q_INVOKABLE QString typeToString(Type t);
private:
    Q_ENUMS(Type)
    QML_WRITABLE_AUTO_PROPERTY(Type, type)
    QML_WRITABLE_AUTO_PROPERTY(QString, number)
};

class Contact : public QObject
{
    Q_OBJECT
    QML_WRITABLE_AUTO_PROPERTY(QString, uid)
    QML_WRITABLE_AUTO_PROPERTY(QString, addressbook)
    QML_WRITABLE_AUTO_PROPERTY(QString, firstname)
    QML_WRITABLE_AUTO_PROPERTY(QString, lastname)
    QML_WRITABLE_AUTO_PROPERTY(QString, nickname)
    QML_WRITABLE_AUTO_PROPERTY(QString, avatar)
    QML_WRITABLE_AUTO_PROPERTY(ContactAddress *, address)
    Q_PROPERTY(QObject *emailAddresses READ emailAddresses NOTIFY modelsChanged)
    Q_PROPERTY(QObject *contactNumbers READ contactNumbers NOTIFY modelsChanged)
    Q_PROPERTY(int sortString READ sortString CONSTANT)
    Q_PROPERTY(QString section READ section CONSTANT)
    Q_PROPERTY(QString initials READ initials CONSTANT)

public:
    explicit Contact(QObject *parent = 0);
    QObject *emailAddresses() const;
    QObject *contactNumbers() const;
    bool hasEmailAddress() { return !m_emailAddresses->isEmpty(); }
    bool hasPhoneNumber() { return !m_contactNumbers->isEmpty(); }
    QList<ContactEmail *> emailList() { return m_emailAddresses->toList(); }
    QList<ContactPhone *> phoneList() { return m_contactNumbers->toList(); }
    int sortString();
    QString section();
    QString initials();

    void addContactEmail(ContactEmail *c);
    void removeContactEmail(const int &idx);
    void addContactNumber(ContactPhone *c);
    void addContactAddress(ContactAddress *c);

    static QString createUid(const QString &abook, const QString &uid) {
        return QString("%1/%2").arg(abook, uid);
    }

    bool operator==(const Contact *contact);
    bool operator==(const ContactId &uid);

Q_SIGNALS:
    void modelsChanged();

private:
    QQmlObjectListModel<ContactEmail> *m_emailAddresses;
    QQmlObjectListModel<ContactPhone> *m_contactNumbers;
};

#endif // CONTACT_H
