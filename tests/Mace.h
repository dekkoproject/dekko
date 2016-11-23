#ifndef MACE_H
#define MACE_H

#include <QObject>
#include <QPointer>
#include <QVariantList>


namespace Mace {

class Runner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit Runner(QObject *parent = 0);
    ~Runner();

    void include(QObject *tests);

    void include(const QString &qmlTests);

    template <typename T>
    void include();

    int count() const;

    bool run(QStringList arguments);

signals:
    void countChanged();

private:
    bool runTests(QObject *testObject, QStringList &args);
    bool runTests(const QString &qmlTestPath, QStringList &arguments);

    QVariantList m_tests;
};

template<typename T>
void Runner::include()
{
    include(new T());
}

}


#endif // MACE_H
