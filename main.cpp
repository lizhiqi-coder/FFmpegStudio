#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include "gl_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
     GLWidget w;
    w.show();

    return a.exec();
}
