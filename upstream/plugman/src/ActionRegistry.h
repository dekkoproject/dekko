#ifndef ACTIONREGISTRY_H
#define ACTIONREGISTRY_H

#include <QObject>
#include <QQmlListProperty>

class ActionRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> defaultActions READ defaultActions)
    Q_PROPERTY(QList<QObject *> actions READ actions NOTIFY actionsChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY locationChanged)

public:
    explicit ActionRegistry(QObject *parent = 0);
    ~ActionRegistry();

    QQmlListProperty<QObject> defaultActions();
    QList<QObject *> actions() const;
    QString location() const;

signals:
    void actionsChanged(QList<QObject *> actions);
    void locationChanged(QString location);

public slots:
    void setLocation(QString location);

private slots:
    void loadActions();

private:
    QList<QObject *> m_actions;
    QList<QObject *> m_defaults;
    QString m_location;
};

#endif // ACTIONREGISTRY_H
