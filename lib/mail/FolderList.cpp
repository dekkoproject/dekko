#include "FolderList.h"

FolderList::FolderList(QObject *parent) : QObject(parent), m_model(0)
{
    m_model = new QQmlObjectListModel<Folder>(this);
    emit modelChanged();
    reset();
}

void FolderList::reset()
{

}

