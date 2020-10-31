#include "testwidget.hpp"

#include <QtWidgets/QApplication>

int main(int argc,char* argv[])
{
    QApplication app{argc,argv};
    TestWidget w;
    w.show();
    return app.exec();
}
