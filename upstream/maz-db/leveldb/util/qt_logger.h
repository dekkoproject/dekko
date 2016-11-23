#ifndef STORAGE_LEVELDB_UTIL_QT_LOGGER_H_
#define STORAGE_LEVELDB_UTIL_QT_LOGGER_H_

#include "leveldb/env.h"
#include <QDateTime>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QThread>

namespace leveldb {

class QtLogger : public Logger {
private:
    QFile file;
    QTextStream log;

public:
    QtLogger(QString fname) : file(fname), log(&file)
    {
        file.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    virtual ~QtLogger() {
        log.flush();
        file.close();
    }

    virtual void Logv(const char* format, va_list ap) {
        const uint64_t thread_id = (qintptr)QThread::currentThreadId();
        QString formated;
        formated = formated.vsprintf(format, ap);

        log << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) << " " << thread_id << " " << formated << '\n';
    }
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_UTIL_QT_LOGGER_H_
