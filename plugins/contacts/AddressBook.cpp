#include "AddressBook.h"
#include <QDebug>
#include <Helpers.h>

AddressBook::AddressBook(QObject *parent) : QObject(parent),
    m_plugin(0), m_model(0), m_listTask(0)
{
    m_model = new QQmlObjectListModel<Contact>(this);
    emit modelChanged();
}

AddressBook::AddressBook(QObject *parent, ContactsPlugin *plugin) :
    QObject(parent), m_plugin(plugin), m_model(0), m_listTask(0)
{
    DELAY_EMIT_NOARG(this, pluginChanged);
    m_model = new QQmlObjectListModel<Contact>(this);
    emit modelChanged();
    DELAY_CALL_NOARG(this, init);
}

QString AddressBook::name() const
{
    return m_plugin->addressBookName();
}

bool AddressBook::canRead() const
{
    return (m_plugin->capabilities() & ContactsPlugin::ReadContact);
}

bool AddressBook::canWrite() const
{
    return (m_plugin->capabilities() & ContactsPlugin::WriteContact);
}

bool AddressBook::canRemove() const
{
    return (m_plugin->capabilities() & ContactsPlugin::RemoveContact);
}

void AddressBook::init()
{
    qDebug() << "Loading addressbook for the first time";
    load();
}

void AddressBook::load()
{
    m_listTask = m_plugin->list(Qt::AscendingOrder);
    connect(m_listTask, &ContactListTask::listComplete, [&](){
        qDebug() << "LIST COMPLETE";
        m_model->append(m_listTask->list());
        DELAY_EMIT_NOARG(this, modelChanged);
        disconnect(m_listTask, 0, this , 0);
        delete m_listTask;
        m_listTask = Q_NULLPTR;
    });
    connect(m_listTask, &ContactListTask::listFailed, [&](){
        qDebug() << "LIST FAILED";
        disconnect(m_listTask, 0, this , 0);
        delete m_listTask;
        m_listTask = Q_NULLPTR;
    });
    m_listTask->run();
}

void AddressBook::reload()
{
    clear();
    load();
}

void AddressBook::clear()
{
    m_model->clear();
}
