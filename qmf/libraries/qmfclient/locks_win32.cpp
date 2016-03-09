/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "locks_p.h"
#include "qmaillog.h"
#include "qmailnamespace.h"
#include <windows.h>

// Because we don't guarantee that a server process will start before
// clients, and not restart without them, we need our locking mechanism
// to tolerate the disappearance of processes without cleaning up their
// locking state.
// Win32 semaphores do not support the SEM_UNDO concept, so we have to rely
// on mutexes, which are released by the kernel when their holder terminates
// unexpectedly.

namespace {

QString pathIdentifier(const QString &path, int id)
{
    // Paths are not permitted to include backslash
    QString key(path);
    key.replace(QChar('\\'), QChar('/'));

    // Object names do not need to correspond to paths that actually exist on Win32
    return QString("qmf-%1-%2").arg(key).arg(id);
}

}


class ProcessMutexPrivate
{
public:
    ProcessMutexPrivate(const QString &path);
    ~ProcessMutexPrivate();

    void lock();
    void unlock();

private:    
    HANDLE mutex;
    int count;
};

ProcessMutexPrivate::ProcessMutexPrivate(const QString &path)
    : mutex(NULL), count(0)
{
    mutex = ::CreateMutex(NULL, FALSE, reinterpret_cast<const wchar_t*>(path.utf16()));
    if (mutex == NULL) {
        qWarning() << "Unable to ceate/open mutex " << path << ":" << QMail::lastSystemErrorMessage();
    }
}

ProcessMutexPrivate::~ProcessMutexPrivate()
{
    if (mutex != NULL) {
        if (::CloseHandle(mutex) == FALSE) {
            qWarning() << "Unable to close handle:" << QMail::lastSystemErrorMessage();
        }
        mutex = NULL;
    }
}

void ProcessMutexPrivate::lock()
{
    if (count) {
        // We already have this lock
        ++count;
    } else {
        DWORD rv = ::WaitForSingleObject(mutex, INFINITE);
        if (rv == WAIT_FAILED) {
            qWarning() << "Unable to wait for mutex:" << QMail::lastSystemErrorMessage();
        } else if (rv != WAIT_TIMEOUT) {
            ++count;
        }
    }
}

void ProcessMutexPrivate::unlock()
{
    if ((count > 0) && (--count == 0)) {
        if (::ReleaseMutex(mutex) == FALSE) {
            qWarning() << "Unable to release mutex:" << QMail::lastSystemErrorMessage();
        }
    }
}

ProcessMutex::ProcessMutex(const QString &path, int id)
    : d(new ProcessMutexPrivate(pathIdentifier(path, id)))
{
}

ProcessMutex::~ProcessMutex()
{
    delete d;
}

void ProcessMutex::lock()
{
    return d->lock();
}

void ProcessMutex::unlock()
{
    d->unlock();
}


class ProcessReadLockPrivate
{
public:
    ProcessReadLockPrivate(const QString &path);
    ~ProcessReadLockPrivate();

    void lock();
    void unlock();

    void wait();

private:
    enum { MaxConcurrentReaders = 10 };

    HANDLE mutexes[MaxConcurrentReaders];
    HANDLE mutex;
    int count;
};

ProcessReadLockPrivate::ProcessReadLockPrivate(const QString &path)
    : mutex(NULL), count(0)
{
    for (int i = 0; i < MaxConcurrentReaders; ++i) {
        mutexes[i] = NULL;
    }

    for (int i = 0; i < MaxConcurrentReaders; ++i) {
        QString subPath = path + '-' + QString::number(i);

        mutexes[i] = ::CreateMutex(NULL, FALSE, reinterpret_cast<const wchar_t*>(subPath.utf16()));
        if (mutexes[i] == NULL) {
            qWarning() << "Unable to ceate/open mutex " << subPath << ":" << QMail::lastSystemErrorMessage();
            break;
        }
    }
}

ProcessReadLockPrivate::~ProcessReadLockPrivate()
{
    for (int i = 0; i < MaxConcurrentReaders; ++i) {
        if (mutexes[i] != NULL) {
            if (::CloseHandle(mutexes[i]) == FALSE) {
                qWarning() << "Unable to close handle:" << QMail::lastSystemErrorMessage();
            }
            mutexes[i] = NULL;
        }
    }
}

void ProcessReadLockPrivate::lock()
{
    if (count) {
        // We already have this lock
        ++count;
    } else {
        // Wait for any of the locks
        DWORD rv = ::WaitForMultipleObjects(MaxConcurrentReaders, mutexes, FALSE, INFINITE);
        if (rv == WAIT_FAILED) {
            qWarning() << "Unable to wait for mutex:" << QMail::lastSystemErrorMessage();
        } else {
            ++count;

            if ((rv >= WAIT_OBJECT_0) && (rv < (WAIT_OBJECT_0 + MaxConcurrentReaders))) {
                mutex = mutexes[(rv - WAIT_OBJECT_0)];
            } else if ((rv >= WAIT_ABANDONED_0) && (rv < (WAIT_ABANDONED_0 + MaxConcurrentReaders))) {
                mutex = mutexes[(rv - WAIT_ABANDONED_0)];
            } else {
                qWarning() << "Unexpected multiple wait result:" << rv;
            }
        }
    }
}

void ProcessReadLockPrivate::unlock()
{
    if ((count > 0) && (--count == 0)) {
        if (::ReleaseMutex(mutex) == FALSE) {
            qWarning() << "Unable to release mutex:" << QMail::lastSystemErrorMessage();
        }
    }
}

void ProcessReadLockPrivate::wait()
{
    // Wait for all of the locks
    DWORD rv = ::WaitForMultipleObjects(MaxConcurrentReaders, mutexes, TRUE, INFINITE);
    if (rv == WAIT_FAILED) {
        qWarning() << "Unable to wait for mutex:" << QMail::lastSystemErrorMessage();
    } else if (rv != WAIT_TIMEOUT) {
        // Release all locks
        for (int i = 0; i < MaxConcurrentReaders; ++i) {
            if (::ReleaseMutex(mutexes[i]) == FALSE) {
                qWarning() << "Unable to release mutex:" << QMail::lastSystemErrorMessage();
            }
        }
    }
}

ProcessReadLock::ProcessReadLock(const QString &path, int id)
    : d(new ProcessReadLockPrivate(pathIdentifier(path, id)))
{
}

ProcessReadLock::~ProcessReadLock()
{
    delete d;
}

void ProcessReadLock::lock()
{
    d->lock();
}

void ProcessReadLock::unlock()
{
    d->unlock();
}

void ProcessReadLock::wait()
{
    d->wait();
}

