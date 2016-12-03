#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QTimer>
#include <QVariantMap>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QUrl>
#include <QmlObjectListModel.h>

class DetailBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY detailChanged)
    Q_PROPERTY(QVariant properties READ props WRITE setProps NOTIFY detailChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY detailChanged)
    Q_ENUMS(Type)
    Q_DISABLE_COPY(DetailBase)
public:
    explicit DetailBase(QObject *parent = 0) : QObject(parent){}

    enum Type {
        String,
        DateTime,
        Uri,
        Base64,
        Unknown
    };

    Type type() const;
    void setType(Type t);
    QVariant props() const;
    void setProps(const QVariant &value);
    QVariant value() const;
    void setValue(const QVariant &val);
    void setDetails(const QVariantMap &details);

    QVariantMap toMap() { return m_details; }

signals:
    void detailChanged();

protected:
    QVariantMap m_details;
};


#define DETAIL_LIST(name, type) \
class name : public QQmlObjectListModel< type > { \
    Q_OBJECT \
    Q_DISABLE_COPY( name ) \
public: \
    explicit name (QObject *parent = 0) : QQmlObjectListModel< type >(parent) {} \
    QVariantList toArray() { \
        QVariantList result; \
        foreach( type *t, this->toList()) { \
            result.append(t->toMap()); \
        } \
        return result; \
    } \
};

#define DETAIL_CLASS(name) \
class name : public DetailBase { \
    Q_OBJECT \
    Q_DISABLE_COPY( name ) \
public: \
    explicit name (QObject *parent = 0) : DetailBase(parent) {}; \
};

DETAIL_CLASS(Version)
DETAIL_CLASS(FullName)
DETAIL_CLASS(Name)
DETAIL_CLASS(Birthday)
DETAIL_CLASS(Anniversary)
DETAIL_CLASS(Gender)
DETAIL_CLASS(Organization)
DETAIL_LIST(OrganizationList, Organization)
DETAIL_CLASS(Address)
DETAIL_LIST(AddressList, Address)
DETAIL_CLASS(Telephone)
DETAIL_LIST(TelephoneList, Telephone)
DETAIL_CLASS(Email)
DETAIL_LIST(EmailList, Email)
DETAIL_CLASS(TimeZone)
DETAIL_CLASS(Url)
DETAIL_LIST(UrlList, Url)

class Contact : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString key READ key NOTIFY contactChanged)
    Q_PROPERTY(Version *version READ version NOTIFY contactChanged)
    Q_PROPERTY(FullName *fullName READ fullName NOTIFY contactChanged)
    Q_PROPERTY(Name *name READ name NOTIFY contactChanged)
    Q_PROPERTY(Birthday *birthday READ birthday NOTIFY contactChanged)
    Q_PROPERTY(Gender *gender READ gender NOTIFY contactChanged)
    Q_PROPERTY(Anniversary *anniversary READ anniversary NOTIFY contactChanged)
    Q_PROPERTY(OrganizationList *organizationList READ organizationList NOTIFY contactChanged)
    Q_PROPERTY(AddressList *addressList READ addressList NOTIFY contactChanged)
    Q_PROPERTY(TelephoneList *telephoneList READ telephoneList NOTIFY contactChanged)
    Q_PROPERTY(EmailList *emailList READ emailList NOTIFY contactChanged)
    Q_PROPERTY(TimeZone *timezone READ timezone NOTIFY contactChanged)
    Q_PROPERTY(UrlList *urls READ urls NOTIFY contactChanged)
public:
    explicit Contact(QObject *parent = 0);
    explicit Contact(QObject *parent, const QString &key, const QVariant &value);
    QString key() { return m_key; }
    Version *version();
    FullName *fullName();
    Name *name();
    Birthday *birthday();
    Gender *gender();
    Anniversary *anniversary();
    OrganizationList *organizationList();
    AddressList *addressList();
    TelephoneList *telephoneList();
    EmailList *emailList();
    TimeZone *timezone();
    UrlList *urls();

    Q_INVOKABLE QVariant toVariant();

    void setKeyValue(QString key, QVariant value) {
        m_key = key;
        m_value = value;
        loadContact();
        emit contactChanged();
    }

signals:
    void contactChanged();

private:
    void loadContact();

    QString m_key;
    QVariant m_value;
    Version m_version;
    FullName m_fullname;
    Name m_name;
    Birthday m_birthday;
    Gender m_gender;
    Anniversary m_anniversary;
    OrganizationList m_orgs;
    AddressList m_addressList;
    TelephoneList m_phones;
    EmailList m_emails;
    TimeZone m_tz;
    UrlList m_urls;
};

#endif // CONTACT_H
