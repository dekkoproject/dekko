#include "PluginTask.h"
#include <QDebug>
#include <QTimer>

PluginTask::PluginTask(QObject *parent) : QObject(parent),
    m_deleteWhenDone(false), m_inProgress(false)
{
}

void PluginTask::complete()
{
    m_inProgress = false;
    if (m_deleteWhenDone) {
        deleteLater();
    }
}

bool PluginTask::deleteWhenDone() const
{
    return m_deleteWhenDone;
}

bool PluginTask::running() const
{
    return m_inProgress;
}

void PluginTask::run()
{
    if (m_inProgress) {
        qWarning() << "Task already running!";
        return;
    }
    m_inProgress = true;
    emit runningChanged(m_inProgress);
    QTimer::singleShot(0, this, &PluginTask::start);
}

void PluginTask::kill()
{
    if (m_inProgress) {
        m_inProgress = false;
        emit runningChanged(m_inProgress);
        QTimer::singleShot(0, this, &PluginTask::stop);
    }
}

void PluginTask::setDeleteWhenDone(bool deleteWhenDone)
{
    if (m_deleteWhenDone == deleteWhenDone)
        return;

    m_deleteWhenDone = deleteWhenDone;
    emit deleteWhenDoneChanged(deleteWhenDone);
}
