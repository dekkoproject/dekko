#ifndef IMAPCONFIGURATION_H
#define IMAPCONFIGURATION_H

#include <QObject>

class ImapConfiguration : public QObject
{
    Q_OBJECT
public:
    explicit ImapConfiguration(QObject *parent = 0);

signals:

public slots:
};

#endif // IMAPCONFIGURATION_H