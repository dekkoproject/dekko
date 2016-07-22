#include "ABookTask.h"

ABookEditTask::ABookEditTask(QObject *parent, Contact *contact, const Action &action) :
    ContactEditTask(parent,contact,action), m_abook(0)
{
}

void ABookEditTask::setABook(ABook *a)
{
    m_abook = a;
}

void ABookEditTask::start()
{
    if (m_abook.isNull()) {
        emit failed();
        complete();
        return;
    }
    switch (action) {
    case Add:
    {
        if (m_abook->update(contact)) {
            emit contactAdded();
            complete();
            return;
        }
        break;
    }
    case Update:
    {
        if (m_abook->update(contact)) {
            emit contactUpdated();
            complete();
            return;
        }
        break;
    }
    case Remove:
    {
        if (m_abook->remove(contact)) {
            emit contactRemoved();
            complete();
            return;
        }
        break;
    }
    }
    emit failed();
    complete();
}

void ABookEditTask::stop()
{
    emit failed();
}

void ABookListTask::setABook(ABook *a)
{
    m_abook = a;
}

void ABookListTask::start()
{
    if (m_abook.isNull()) {
        emit listFailed();
        complete();
        return;
    }
    result.clear();
    result = m_abook->list();
    emit listComplete();
    complete();
}

void ABookListTask::stop()
{
    emit listFailed();
    complete();
}
