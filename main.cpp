#include <QApplication>
#include "decision.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Decision w;
    w.show();

    return a.exec();
}
