#include "widget.hpp"

#include <QtWidgets/QApplication>

int main(int argc,char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app{argc,argv};
    Widget w;
    w.show();
    return app.exec();
}
