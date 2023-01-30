#include "Wallpaper.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Wallpaper w;
    w.show();
    return a.exec();
}
