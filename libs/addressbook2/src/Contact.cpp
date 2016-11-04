#include "Contact.h"
#include <QJsonArray>

Contact::Contact(QObject *parent) : QObject(parent)
{
}

Contact::Contact(QObject *parent, const QString &key, const QVariant &value) :
    QObject(parent), m_key(key), m_value(value)
{
    loadContact();
}

QVariant Contact::toVariant() {
    QVariantMap val = m_value.toMap();
    val["version"] = m_version.toMap();
    val["fn"] = m_fullname.toMap();
    val["n"] = m_name.toMap();
    val["bday"] = m_birthday.toMap();
    val["anniversary"] = m_anniversary.toMap();
    val["gender"] = m_gender.toMap();
    val["org"] = m_orgs.toArray();
    val["adr"] = m_addressList.toArray();
    val["email"] = m_emails.toArray();
    val["tel"] = m_phones.toArray();
    val["url"] = m_urls.toArray();
    return val;
}

void Contact::loadContact() {
    QVariantMap val = m_value.toMap();
    if (val.isEmpty()) {
        return;
    }
    if (val.keys().contains(QStringLiteral("version"))) {
        m_version.setDetails(val.value(QStringLiteral("version"), QVariant()).toMap());
    }
    if (val.keys().contains(QStringLiteral("fn"))) {
        m_fullname.setDetails(val.value(QStringLiteral("fn"), QVariant()).toMap());
    }
    if (val.keys().contains(QStringLiteral("n"))) {
        m_name.setDetails(val.value(QStringLiteral("n"), QVariant()).toMap());
    }
    if (val.keys().contains(QStringLiteral("bday"))) {
        m_birthday.setDetails(val.value(QStringLiteral("bday"), QVariant()).toMap());
    }
    if (val.keys().contains(QStringLiteral("anniversary"))) {
        m_anniversary.setDetails(val.value(QStringLiteral("anniversary"), QVariant()).toMap());
    }
    if (val.keys().contains(QStringLiteral("gender"))) {
        m_gender.setDetails(val.value(QStringLiteral("gender"), QVariant()).toMap());
    }
    if (val.keys().contains(QStringLiteral("org"))) {
        QJsonArray orgs = val.value(QStringLiteral("org")).toJsonArray();
        if (!orgs.isEmpty()) {
            for (QVariant &jv : orgs.toVariantList()) {
                Organization *org = new Organization(Q_NULLPTR);
                org->setDetails(jv.toMap());
                m_orgs.append(org);
            }
        }
    }
    if (val.keys().contains(QStringLiteral("adr"))) {
        QJsonArray adrs = val.value(QStringLiteral("adr")).toJsonArray();
        if (!adrs.isEmpty()) {
            for (QVariant &a : adrs.toVariantList()) {
                Address *address = new Address(Q_NULLPTR);
                address->setDetails(a.toMap());
                m_addressList.append(address);
            }
        }
    }
    if (val.keys().contains(QStringLiteral("tel"))) {
        QJsonArray tels = val.value(QStringLiteral("tel")).toJsonArray();
        if (!tels.isEmpty()) {
            for (QVariant &t : tels.toVariantList()) {
                Telephone *tel = new Telephone(Q_NULLPTR);
                tel->setDetails(t.toMap());
                m_phones.append(tel);
            }
        }
    }
    if (val.keys().contains(QStringLiteral("email"))) {
        QJsonArray emails = val.value(QStringLiteral("email")).toJsonArray();
        if (!emails.isEmpty()) {
            for (QVariant &email : emails.toVariantList()) {
                Email *e = new Email(Q_NULLPTR);
                e->setDetails(email.toMap());
                m_emails.append(e);
            }
        }
    }
    if (val.keys().contains(QStringLiteral("url"))) {
        QJsonArray urls = val.value(QStringLiteral("url")).toJsonArray();
        if (!urls.isEmpty()) {
            for (QVariant &url : urls.toVariantList()) {
                Url *u = new Url(Q_NULLPTR);
                u->setDetails(url.toMap());
                m_urls.append(u);
            }
        }
    }
}

Version *Contact::version() { return &m_version; }
FullName *Contact::fullName() { return &m_fullname; }
Name *Contact::name() { return &m_name; }
Birthday *Contact::birthday() { return &m_birthday; }
Gender *Contact::gender() { return &m_gender; }
Anniversary *Contact::anniversary() { return &m_anniversary; }
OrganizationList *Contact::organizationList() { return &m_orgs; }
AddressList *Contact::addressList() { return &m_addressList; }
TelephoneList *Contact::telephoneList() { return &m_phones; }
EmailList *Contact::emailList() { return &m_emails; }
TimeZone *Contact::timezone() { return &m_tz; }
UrlList *Contact::urls() { return &m_urls; }


DetailBase::Type DetailBase::type() const {
    QString type = m_details.value(QStringLiteral("type"), QString()).toString();
    if (type == QStringLiteral("text")) {
        return String;
    } else if (type == QStringLiteral("datetime")) {
        return DateTime;
    } else if (type == QStringLiteral("uri")) {
        return Uri;
    } else if (type == QStringLiteral("base64")) {
        return Base64;
    } else {
        return Unknown;
    }
}

void DetailBase::setType(DetailBase::Type t)
{
    if (type() != t) {
        switch(t) {
        case String:
            m_details["type"] = "text";
            break;
        case DateTime:
            m_details["type"] = "datetime";
            break;
        case Uri:
            m_details["type"] = "uri";
            break;
        case Base64:
            m_details["type"] = "base64";
            break;
        default:
            m_details["type"] = "unknown";
            break;
        }
        emit detailChanged();
    }
}

QVariant DetailBase::props() const {
    QJsonValue val = m_details.value(QStringLiteral("prop"), QString()).toJsonValue();
    if (val.type() == QJsonValue::Array) {
        return val.toArray();
    } else if (val.type() == QJsonValue::Object) {
        return val.toObject();
    } else if (val.type() == QJsonValue::String) {
        return val.toString();
    }
    return QVariant();
}

void DetailBase::setProps(const QVariant &value)
{
    if (props() != value) {
        m_details["prop"] = value;
        emit detailChanged();
    }
}

QVariant DetailBase::value() const {
    switch(type()) {
    case String:
        return m_details.value(QStringLiteral("value"), QString()).toString();
    case DateTime:
        return m_details.value(QStringLiteral("value"), QString()).toDateTime();
    case Uri:
        return m_details.value(QStringLiteral("value"), QUrl()).toUrl();
    case Base64:
        return m_details.value(QStringLiteral("value"), QByteArray()).toByteArray();
    default:
        return QVariant();
    }
}

void DetailBase::setValue(const QVariant &val)
{
    if (val != value()) {
        m_details[QStringLiteral("value")] = val;
        emit detailChanged();
    }
}

void DetailBase::setDetails(const QVariantMap &details) {
    if (!m_details.isEmpty()) {
        m_details.clear();
    }
    m_details = details;
    emit detailChanged();
}
