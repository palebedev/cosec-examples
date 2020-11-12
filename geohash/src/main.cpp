#include "mainwidget.hpp"

#include <QtWidgets/QApplication>

int main(int argc,char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app{argc,argv};
    MainWidget mw;
    mw.show();
    return app.exec();
}
