#include <QCoreApplication>
#include "Dekko.h"

int main(int argc, char** argv)
{
    Dekko dekko(argc, argv);

    if (!dekko.setup()) {
        return 0;
    }

    return dekko.exec();
}
