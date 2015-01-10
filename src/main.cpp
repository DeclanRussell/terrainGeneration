#include <QApplication>
#include "mainwindow.h"
#include "QFile"

int main(int argc, char **argv)
{
    QApplication app(argc,argv);
    MainWindow w;
    QFile file("stylesheet/darkOrange");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    w.setStyleSheet(styleSheet);
    w.setWindowTitle(QString("Terrain Generator"));
    w.show();
    app.exec();
}
