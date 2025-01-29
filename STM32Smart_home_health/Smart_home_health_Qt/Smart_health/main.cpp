#include "log_in.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Widget w;
    // w.show();

    log_in login;
    login.show();

    return a.exec();
}
