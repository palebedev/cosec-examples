#include "mainwindow.hpp"

#include <QtWidgets/QApplication>

int main(int argc,char* argv[])
{
    QApplication app{argc,argv};
    MainWindow mw;
    mw.show();
    return app.exec();
}
