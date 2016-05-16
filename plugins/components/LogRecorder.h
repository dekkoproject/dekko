#ifndef LOGRECORDER_H
#define LOGRECORDER_H

#include <QObject>
#include <QmlObjectListModel.h>
//#include <QQmlAutoPropertyHelpers.h>
#include <QtQuick>
#include <QQmlEngine>
#include <QJSEngine>

class LogMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    QString m_message;

public:
    explicit LogMessage(QObject *parent = 0) : QObject(parent){}
    QString message() const
    {
        return m_message;
    }
public slots:
    void setMessage(QString message)
    {
        if (m_message == message)
            return;

        m_message = message;
        emit messageChanged(message);
    }
signals:
    void messageChanged(QString message);
};

class LogRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *model READ model NOTIFY modelChanged)
    Q_ENUMS(Type)
public:
    explicit LogRecorder(QObject *parent = 0);
    enum Type {
        INFO,
        STATUS,
        WARNING,
        ERROR
    };
    QObject *model() { return m_model; }
    static QObject *factory(QQmlEngine *engine, QJSEngine *scriptEngine);

signals:
    void modelChanged();

public slots:
    void logMessage(const QString &location, const int &type, const QString &message);

private:
    QString formatMessage(const QString &location, const int &type, const QString &message);
    QQmlObjectListModel<LogMessage> *m_model;
};

#endif // LOGRECORDER_H
