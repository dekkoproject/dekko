#ifndef PLUGINTASK_H
#define PLUGINTASK_H

#include <QObject>

class PluginTask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool deleteWhenDone READ deleteWhenDone WRITE setDeleteWhenDone NOTIFY deleteWhenDoneChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    bool m_deleteWhenDone;
    bool m_inProgress;

public:
    bool deleteWhenDone() const;
    bool running() const;

signals:
    void deleteWhenDoneChanged(bool deleteWhenDone);
    void runningChanged(bool running);

public slots:
    void run(); // run the task
    void kill(); // stop the task
    void setDeleteWhenDone(bool deleteWhenDone);

protected:
    explicit PluginTask(QObject *parent = 0);

protected slots:
    virtual void start() = 0;
    virtual void stop() = 0;
    void complete();
};

#endif // PLUGINTASK_H
