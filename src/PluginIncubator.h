#ifndef PLUGININCUBATOR_H
#define PLUGININCUBATOR_H

#include <QObject>
#include <QQmlIncubator>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QUrl>

class PluginIncubator : public QObject, public QQmlIncubator
{
    Q_OBJECT
public:
    explicit PluginIncubator(QObject *parent = 0);

    void setSourceUrl(QQmlEngine *engine, const QUrl &source);

signals:
    void objectReady();
    void error();

    // QQmlIncubator interface
protected:
    virtual void statusChanged(Status status) override;

private:
    QQmlComponent *m_comp;
};

typedef QList<PluginIncubator *> IncubatorList;

#endif // PLUGININCUBATOR_H
