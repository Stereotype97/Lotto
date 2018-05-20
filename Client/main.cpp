#include "mainwindow.h"
#include <QApplication>
#include <entrywindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
    EntryWindow entry;
    entry.show();

    return a.exec();
}
