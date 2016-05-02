#ifndef EMAILPROVIDER_H
#define EMAILPROVIDER_H

#include <QObject>

class EmailProvider : public QObject
{
    Q_OBJECT
public:
    explicit EmailProvider(QObject *parent = 0);

signals:

public slots:
};

#endif // EMAILPROVIDER_H
