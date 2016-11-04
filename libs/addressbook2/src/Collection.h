#ifndef COLLECTION_H
#define COLLECTION_H

#include <QObject>
#include <QmlObjectListModel.h>
#include <MazDB.h>

class Contact;
class Collection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ collection CONSTANT)
    Q_PROPERTY(QString source READ source CONSTANT)
    Q_PROPERTY(QString startKey READ startKey CONSTANT)
    Q_PROPERTY(QString endKey READ endKey CONSTANT)

public:
    explicit Collection(QObject *parent = 0);
    explicit Collection(QObject *parent, const QString &source, const QString &collection);

    QString collection() const;
    QString source() const;
    QString startKey() const;
    QString endKey() const;


    void keyChanged(const QString &key, const QVariant &value);
    void keyRemoved(const QString &key);

private:
    void load();
    QString m_source;
    QString m_collection;
};

#endif // COLLECTION_H
