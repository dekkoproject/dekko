#ifndef FOLDERLIST_H
#define FOLDERLIST_H

#include <QObject>
#include "Folder.h"

class FolderList : public QObject
{
    Q_OBJECT
public:
    explicit FolderList(QObject *parent = 0);

signals:

public slots:
};

#endif // FOLDERLIST_H
