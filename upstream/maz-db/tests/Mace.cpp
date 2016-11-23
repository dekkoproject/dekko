#include "Mace.h"
#include <QTest>
#include <QDebug>
#include <QStringList>
#include <QMetaObject>
#include <QMetaMethod>
#include <QtQuickTest/QtQuickTest>


static QPointer<Mace::Runner> s_instance;

Mace::Runner::Runner(QObject *parent) : QObject(parent)
{
    if (s_instance.isNull()) {
        s_instance = this;
    }

}

Mace::Runner::~Runner()
{
    for(auto test : m_tests) {
        if (auto object = test.value<QObject*>()) {
            delete object;
        }
    }
    m_tests.clear();
    if (s_instance == this) {
        s_instance = 0;
    }
}

void Mace::Runner::include(QObject *tests)
{
    m_tests << QVariant::fromValue<QObject *>(tests);
    emit countChanged();
}

void Mace::Runner::include(const QString &qmlTests)
{
    m_tests << QVariant(qmlTests);
    emit countChanged();
}

int Mace::Runner::count() const
{
    return m_tests.size();
}

bool Mace::Runner::run(QStringList arguments)
{
    qDebug() << "Arguments:" << arguments;
    qDebug() << "Tests: " << m_tests;
    bool testFailed = false;
    foreach(const QVariant &test, m_tests) {
        QStringList runArgs = arguments;
        if (testFailed) {
            break;
        }
        if (auto object = test.value<QObject *>()) {
            testFailed |= runTests(object, runArgs);
        } else if (test.type() == (int)QMetaType::QString) {
            testFailed |= runTests(test.toString(), runArgs);
        } else {
            qDebug() << "unkown type:" << test.type();
        }
    }
    return testFailed;
}

bool Mace::Runner::runTests(QObject *testObject, QStringList &args)
{
    QStringList tests;
    QStringList qtestArgs;
    qtestArgs << args.takeAt(0);
    for (const QString &arg : args) {
        if (arg.startsWith('-')) {
            qtestArgs << args;
        } else {
            tests << arg;
        }
    }
    bool shouldRun = tests.size() > 0 ? false : true;
    auto metaObject = testObject->metaObject();
    for (const QString &test : tests) {
        if (test == metaObject->className()) {
            shouldRun = true;
            break;
        }

        for (int i = 0; i < metaObject->methodCount(); ++i) {
            if (metaObject->method(i).name() == test) {
                qtestArgs << test;
                shouldRun = true;
            }
        }
    }
    if (!shouldRun) {
        return false;
    }
    return QTest::qExec(testObject, qtestArgs);
}

bool Mace::Runner::runTests(const QString &qmlTestPath, QStringList &arguments)
{
    QStringList args(arguments);
    QString executable;
    if (!args.isEmpty()) {
        executable = args.takeAt(0);
    }
    QStringList testcases = arguments.filter(QRegExp("::"));

    QStringList tmp; // Filter all "-" parameter
    foreach (QString arg, args) {
        if (arg.indexOf("-") != 0) {
            tmp << arg;
        }
    }

    args = tmp;

    if (args.size() !=0 && testcases.size() == 0) {
        return false;
    }

    QStringList paths;
    paths << qmlTestPath;
//    paths << m_importPaths; TODO

    char **s = (char**) malloc(sizeof(char*) * (10 + testcases.size() + paths.size() * 2));
    int idx = 0;
    s[idx++] = executable.toUtf8().data();

    foreach (QString p , paths) {
        s[idx++] = strdup("-import");
        s[idx++] = strdup(p.toUtf8().data());
    }

    foreach( QString testcase,testcases) {
        s[idx++] = strdup(testcase.toUtf8().data());
    }
    s[idx++] = 0;

    const char *name = "Mace";
    const char *source = strdup(qmlTestPath.toUtf8().data());

    bool error = quick_test_main( idx-1, s, name, source);
    free(s);

    return error;
}
