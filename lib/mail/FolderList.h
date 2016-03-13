#ifndef FOLDERLIST_H
#define FOLDERLIST_H

#include <QObject>
#include <QmlObjectListModel.h>
#include "Folder.h"

class FolderList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
public:
    explicit FolderList(QObject *parent = 0);

    QObject *model() { return m_model; }

signals:
    void modelChanged();

public slots:

    void reset();

private:
    QQmlObjectListModel<Folder> *m_model;
};

#endif // FOLDERLIST_H
