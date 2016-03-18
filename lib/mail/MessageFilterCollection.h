#ifndef MESSAGEFILTERCOLLECTION_H
#define MESSAGEFILTERCOLLECTION_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "MessageSet.h"

// This component create a list of message sets based on some predefined filters
class MessageFilterCollection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *children READ children NOTIFY childrenChanged)
    Q_PROPERTY(Filter filter READ filter WRITE setFilter NOTIFY filterChanged)

    Q_ENUMS(Filter)
public:
    explicit MessageFilterCollection(QObject *parent = 0);

    enum Filter {
        None,
        // Creates a list of merged standard folders
        // The StandardFolderSet has a childAccounts list model that can be used to select
        // a specific account inbox.
        StandardFolders
    };

    QObject *children() { return m_children; }

    Filter filter() const { return m_filter; }

signals:

    void filterChanged(Filter filter);
    void childrenChanged();

public slots:

    void setFilter(Filter filter);
    void reset();

protected:
    void createStandardFolderCollection();

private:
    QQmlObjectListModel<MessageSet> *m_children;
    Filter m_filter;
    bool m_firstRun;
};

#endif // MESSAGEFILTERCOLLECTION_H
