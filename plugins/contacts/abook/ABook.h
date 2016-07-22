#ifndef ABOOK_H
#define ABOOK_H

#include <QObject>
#include <QSettings>
#include <Contact.h>

struct ABookKeys {
    static const QString name, email, nickname, avatar;
    static const QString address, city, state, zip, country;
    static const QString phone, workphone, fax, mobile;
    static const QString formatGroup;
};

class ABook : public QObject
{
    Q_OBJECT
public:
    explicit ABook(QObject *parent = 0);
    // abook version
    QString version() const;
    QList<Contact *> list();
    bool add(Contact *contact);
    bool remove(Contact *contact);
    bool update(Contact *contact);
    bool import(const QByteArray &vcard);

private:
    void createSearchIndex();
    void writeContact(Contact *contact, QSettings *abook);
    void readContact(Contact *contact, QSettings *abook);
    // We use the value as the key here
    //which can run the query regex
    QHash<QString, QPair<int, QString> > m_index;
    QSettings *m_abook;
};

#endif // ABOOK_H
