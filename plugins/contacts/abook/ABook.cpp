#include "ABook.h"
#include <QStringBuilder>
#include <Paths.h>
#include <emailvalidator.h>

const QString ABookKeys::name = QStringLiteral("name");
const QString ABookKeys::email = QStringLiteral("email");
const QString ABookKeys::nickname = QStringLiteral("nick");
const QString ABookKeys::avatar = QStringLiteral("photo");
const QString ABookKeys::address = QStringLiteral("address");
const QString ABookKeys::city = QStringLiteral("city");
const QString ABookKeys::state = QStringLiteral("state");
const QString ABookKeys::zip = QStringLiteral("zip");
const QString ABookKeys::country = QStringLiteral("country");
const QString ABookKeys::phone = QStringLiteral("phone");
const QString ABookKeys::workphone = QStringLiteral("workphone");
const QString ABookKeys::fax = QStringLiteral("fax");
const QString ABookKeys::mobile = QStringLiteral("mobile");
const QString ABookKeys::formatGroup = QStringLiteral("formatGroup");

ABook::ABook(QObject *parent) : QObject(parent),
    m_abook(0)
{
    // if we are on desktop and the user has abook setup in it's default location then
    // we will use that.
    QString homePath = Paths::homeLocation().append(QStringLiteral("/.abook/addressbook"));
    if (QFile::exists(homePath)) {
        qDebug() << "Using abook in home dir";
        m_abook = new QSettings(homePath, QSettings::IniFormat);
    } else {
        qDebug() << "Using Dekko abook location";
        m_abook = new QSettings(Paths::dataLocationForFile(QStringLiteral("/contacts/.abook/addressbook")), QSettings::IniFormat);
    }
    m_abook->setIniCodec("UTF-8");
    createSearchIndex();
}

QString ABook::version() const
{
    if (m_abook->childGroups().contains(ABookKeys::formatGroup)) {
        m_abook->beginGroup(ABookKeys::formatGroup);
        QString version = m_abook->value(QStringLiteral("version")).toString();
        m_abook->endGroup();
        return version;
    } else {
        return QStringLiteral("0.1");
    }
}

QList<Contact *> ABook::list()
{
    QStringList currentGroups = m_abook->childGroups();
    currentGroups.removeAll(ABookKeys::formatGroup);
    QList<Contact *> list;
    Q_FOREACH(const QString &group, currentGroups) {
        Contact *c = new Contact(this);
        c->set_addressbook("abook");
        c->set_id(group);
        m_abook->beginGroup(group);
        readContact(c, m_abook);
        m_abook->endGroup();
        list.append(c);
    }
    return list;
}

bool ABook::add(Contact *contact)
{
    if (contact->addressbook() != QStringLiteral("abook")) {
        return false;
    }
    if (m_abook->childGroups().contains(contact->id())) {
        return update(contact);
    }
    QStringList currentGroups = m_abook->childGroups();
    currentGroups.removeAll(ABookKeys::formatGroup);
    contact->set_id(QString::number(currentGroups.count() + 1));
    contact->set_addressbook("abook");
    m_abook->beginGroup(contact->id());
    writeContact(contact, m_abook);
    m_abook->endGroup();
    m_abook->sync();
    createSearchIndex();
    return false;
}

// This isn't ideal as we don't have the full abook in memory
// so we can't just wipe and write after the remove.
// So instead we build a map of all remaining contacts and rewrite
// their group name (index). THis is where QSettings really sucks as
// you can't change a groups name. Try not to squirm too much looking below :-S
bool ABook::remove(Contact *contact)
{
    if (contact->addressbook() != QStringLiteral("abook")) {
        return false;
    }
    if (!m_abook->childGroups().contains(contact->id())) {
        return false;
    }
    QStringList currentGroups = m_abook->childGroups();
    currentGroups.removeAll(ABookKeys::formatGroup);

    typedef QHash<int, QPair<QString, QVariant> > GroupHash;

    // build a hash table of key/values per group
    GroupHash hash;
    bool seen = false; // if seen we need to drop the new index by 1
    Q_FOREACH(const QString &group, currentGroups) {
        if (group == contact->id()) {
            seen = true;
            continue;
        }
        m_abook->beginGroup(group);
        Q_FOREACH(const QString &key, m_abook->childKeys()) {
            hash.insertMulti(
                              (seen ? group.toInt() - 1 : group.toInt()),
                              qMakePair(key, m_abook->value(key))
                            );
        }
        m_abook->endGroup();
    }
    // wipe everything. We can't just call QSettings::clear here
    // as we want to keep the [format] group (if it exists) intact.
    Q_FOREACH(const QString &group, currentGroups) {
        m_abook->beginGroup(group);
        m_abook->remove("");
        m_abook->endGroup();
    }

    // Now write back from the hash table, as we used insertMulti
    // we need to do this on the unique keys.
    Q_FOREACH(const int &key, hash.uniqueKeys()) {
        m_abook->beginGroup(QString::number(key));
        GroupHash::const_iterator i = hash.find(key);
        while (i != hash.end() && i.key() == key) {
            QPair<QString, QVariant> value = i.value();
            m_abook->setValue(value.first, value.second);
            ++i;
        }
        m_abook->endGroup();
    }
    // and were done
    m_abook->sync();
    // update the search index as that will be out of sync now.
    createSearchIndex();
    return true;
}

bool ABook::update(Contact *contact)
{
    if (contact->addressbook() != QStringLiteral("abook")) {
        return false;
    }
    if (!m_abook->childGroups().contains(contact->id())) {
        return false;
    }
    m_abook->beginGroup(contact->id());
    writeContact(contact, m_abook);
    m_abook->endGroup();
    m_abook->sync();
    createSearchIndex();
    return true;
}

bool ABook::import(const QByteArray &vcard)
{
    Q_ASSERT(false);
    return false;
}

void ABook::createSearchIndex()
{
//    m_index.clear();
//    QStringList groups = m_abook->childGroups();
//    groups.removeAll(ABookKeys::formatGroup);
//    Q_FOREACH(const QString &group, groups) {
//        m_abook->beginGroup(group);
//        const bool hasName = m_abook->childKeys().contains(QStringLiteral("name"));
//        const bool hasEmail = m_abook->childKeys().contains(QStringLiteral("email"));
//        if (hasName) {
//            m_index.insert(
//                            m_abook->value(QStringLiteral("name")).toString(),
//                            qMakePair(group.toInt(), QStringLiteral("name"));
//                          );
//        }
//        if (hasEmail) {
//            QString emails = m_abook->value(QStringLiteral("email")).toString();
//            Q_FOREACH(const QString &e, emails.split(QChar(','))) {
//                if (EmailValidator::instance()->validate(e)) {
//                    m_index.insert(e, qMakePair(group.toInt(), QStringLiteral("email")));
//                }
//            }
//        }
//        m_abook->endGroup();
//    }
}

void ABook::writeContact(Contact *contact, QSettings *abook)
{
    abook->remove(""); // clear and re-write them all
    if (!contact->firstname().isEmpty()) {
        QString name = contact->firstname();
        if (!contact->lastname().isEmpty()) {
            name = (name % " " % contact->lastname());
        }
        abook->setValue(ABookKeys::name, name);
    }
    if (contact->hasEmailAddress()) {
        QStringList emails;
        foreach(ContactEmail *email, contact->emailList()) {
            emails << email->address();
        }
        abook->setValue(ABookKeys::email, emails.join(QChar(',')));
    }
    if (contact->hasPhoneNumber()) {
        foreach(ContactPhone *phone, contact->phoneList()) {
            switch(phone->type()) {
            case ContactPhone::Home:
                abook->setValue(ABookKeys::phone, phone->number());
                break;
            case ContactPhone::Work:
                abook->setValue(ABookKeys::workphone, phone->number());
                break;
            case ContactPhone::Fax:
                abook->setValue(ABookKeys::fax, phone->number());
                break;
            case ContactPhone::Mobile:
                abook->setValue(ABookKeys::mobile, phone->number());
                break;
            }
        }
    }
    auto updateIfNotEmpty = [&](const QString &key, const QString &newValue) {
        if (!newValue.isEmpty()) {
            abook->setValue(key, newValue);
        }
    };
    updateIfNotEmpty(ABookKeys::address, contact->address()->street());
    updateIfNotEmpty(ABookKeys::city, contact->address()->city());
    updateIfNotEmpty(ABookKeys::state, contact->address()->state());
    updateIfNotEmpty(ABookKeys::zip, contact->address()->zip());
    updateIfNotEmpty(ABookKeys::country, contact->address()->country());
    updateIfNotEmpty(ABookKeys::avatar, contact->avatar());
    updateIfNotEmpty(ABookKeys::nickname, contact->nickname());
}

void ABook::readContact(Contact *contact, QSettings *abook)
{
    QStringList keys = abook->childKeys();
    if (keys.contains(ABookKeys::name)) {
        contact->set_firstname(abook->value(ABookKeys::name).toString());
    }
    if (keys.contains(ABookKeys::email)) {
        QString emails = abook->value(ABookKeys::email).toString();
        Q_FOREACH(const QString &email, emails.split(QChar(','))) {
            QString name;
            if (keys.contains(ABookKeys::name)) {
                name = abook->value(ABookKeys::name).toString();
            }
            ContactEmail *addr = new ContactEmail(Q_NULLPTR);
            addr->set_address(email);
            contact->addContactEmail(addr);
        }
    }
    // Address Details
    ContactAddress *addr = new ContactAddress(contact);
    if (keys.contains(ABookKeys::address)) {
        addr->set_street(abook->value(ABookKeys::address).toString());
    }

    if (keys.contains(ABookKeys::city)) {
        addr->set_city(abook->value(ABookKeys::city).toString());
    }

    if (keys.contains(ABookKeys::state)) {
        addr->set_state(abook->value(ABookKeys::state).toString());
    }

    if (keys.contains(ABookKeys::zip)) {
        addr->set_zip(abook->value(ABookKeys::zip).toString());
    }

    if (keys.contains(ABookKeys::country)) {
        addr->set_country(abook->value(ABookKeys::country).toString());
    }
    contact->set_address(addr);

    // Contact details
    if (keys.contains(ABookKeys::phone)) {
        ContactPhone *c = new ContactPhone(this);
        c->set_type(ContactPhone::Home);
        c->set_number(abook->value(ABookKeys::phone).toString());
        contact->addContactNumber(c);
    }

    if (keys.contains(ABookKeys::workphone)) {
        ContactPhone *c = new ContactPhone(this);
        c->set_type(ContactPhone::Work);
        c->set_number(abook->value(ABookKeys::workphone).toString());
        contact->addContactNumber(c);
    }

    if (keys.contains(ABookKeys::fax)) {
        ContactPhone *c = new ContactPhone(this);
        c->set_type(ContactPhone::Fax);
        c->set_number(abook->value(ABookKeys::fax).toString());
        contact->addContactNumber(c);
    }

    if (keys.contains(ABookKeys::mobile)) {
        ContactPhone *c = new ContactPhone(this);
        c->set_type(ContactPhone::Mobile);
        c->set_number(abook->value(ABookKeys::mobile).toString());
        contact->addContactNumber(c);
    }
    // Other shmuck
    if (keys.contains(ABookKeys::nickname)) {
        contact->set_nickname(abook->value(ABookKeys::nickname).toString());
    }
    if (keys.contains(QStringLiteral("url"))) {
        // nothing for this yet
    }
    if (keys.contains(ABookKeys::avatar)) {
        contact->set_avatar(abook->value(ABookKeys::avatar).toString());
    }
}
