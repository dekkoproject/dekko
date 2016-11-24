#ifndef HELPERS
#define HELPERS
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QVariantMap>


#define VALID_NAME "abook1"
#define VALID_PATH "/tmp/path"

namespace Helpers
{

static QJsonObject createContact(const QString &id)
{
    QJsonObject contact;
    contact.insert("addressbook", QStringLiteral(VALID_NAME));
    contact.insert("id", id);
    contact.insert("path", QStringLiteral(VALID_PATH));

    QJsonObject name;
    name.insert("prefixes", QJsonArray() << QStringLiteral("Sir"));
    name.insert("forenames", QJsonArray() << QStringLiteral("Dan") << QStringLiteral("The"));
    name.insert("surname", QStringLiteral("Man"));
    name.insert("suffixes", QJsonArray() << QStringLiteral("Esq"));
    name.insert("additional", QJsonArray());
    contact.insert("name", name);

    // Test email objects
    QJsonArray emails;
    QJsonObject email1;
    email1.insert("type", "internet");
    email1.insert("address", "test1@example.org");
    emails.append(email1);
    QJsonObject email2;
    email2.insert("type", "work");
    email2.insert("address", "test2@example.org");
    emails.append(email2);
    contact.insert("email", emails);

    QJsonArray phones;
    QJsonObject phone1;
    phone1.insert("type", "voice");
    phone1.insert("address", "012345678");
    phones.append(phone1);
    QJsonObject phone2;
    phone2.insert("type", "void");
    phone2.insert("address", "876543210");
    phones.append(phone2);
    contact.insert("phone_number", phones);

    return contact;
}

static QVariantMap abookConfig() {
    QVariantMap m_config;
    m_config.insert("name", VALID_NAME);
    m_config.insert("path", VALID_PATH);
    m_config.insert("can_read", true);
    m_config.insert("can_write", false);
    m_config.insert("can_delete", true);
    return m_config;
}

}

#endif // HELPERS

