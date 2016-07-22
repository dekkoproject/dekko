#ifndef ABOOKTASK_H
#define ABOOKTASK_H

#include <QObject>
#include <QPointer>
#include <ContactsPlugin.h>
#include "ABook.h"

class ABookEditTask : public ContactEditTask
{
    Q_OBJECT
public:
    ABookEditTask(QObject *parent, Contact *contact, const Action &action);

    void setABook(ABook *a);
    // PluginTask interface
protected slots:
    void start();
    void stop();

private:
    QPointer<ABook> m_abook;
};

class ABookListTask : public ContactListTask
{
    Q_OBJECT
public:
    ABookListTask(QObject *parent, const Qt::SortOrder &sortOrder) :
        ContactListTask(parent, sortOrder)
    {}

    void setABook(ABook *a);
    // PluginTask interface
protected slots:
    void start();
    void stop();
private:
    QPointer<ABook> m_abook;
};

#endif // ABOOKTASK_H
