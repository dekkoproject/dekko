#include "PluginIncubator.h"
#include <QDebug>

PluginIncubator::PluginIncubator(QObject *parent) : QObject(parent),
    QQmlIncubator(QQmlIncubator::Asynchronous), m_comp(Q_NULLPTR)
{
}

void PluginIncubator::setSourceUrl(QQmlEngine *engine, const QUrl &source)
{
    m_comp = new QQmlComponent(engine, source, this);
    if (m_comp->isError()) {
        qDebug() << "Comp failed";
        emit error();
        return;
    }
    m_comp->create(*this);
}

void PluginIncubator::statusChanged(QQmlIncubator::Status status)
{
    switch (status) {
    case QQmlIncubator::Null:
        break;
    case QQmlIncubator::Ready:
        qDebug() << "Incubator finished and ready to hatch.";
        emit objectReady();
        break;
    case QQmlIncubator::Loading:
        qDebug() << "Incubating plugin";
        break;
    case QQmlIncubator::Error:
        qDebug() << "Incubation Error";
        emit error();
        break;
    default:
        break;
    }
}
