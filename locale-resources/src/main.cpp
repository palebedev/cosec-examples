#include "mainwindow.hpp"

#include <QtWidgets/QApplication>

int main(int argc,char* argv[])
{
    QApplication app{argc,argv};
    app.setApplicationName(QStringLiteral("locale-resources"));
    app.setApplicationVersion(QStringLiteral("1.0"));
    MainWindow w;
    w.show();
    return app.exec();
}
