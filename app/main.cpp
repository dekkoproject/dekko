#include <QCoreApplication>
#include "Dekko.h"

int main(int argc, char** argv)
{
    Dekko dekko(argc, argv);

    qputenv("QMF_DATA", QByteArrayLiteral("/home/dan/.dekkoqmf"));

    if (!dekko.setup()) {
        return 0;
    }

    return dekko.exec();
}
