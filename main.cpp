#include <QApplication>
#include "decision.h"
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QApplication a(argc, argv);

    Decision w;
    w.show();

    return a.exec();
}
