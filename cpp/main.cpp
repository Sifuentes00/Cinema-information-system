#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    qRegisterMetaType<Film>("Film");
    qRegisterMetaType<Action>("Action");
    qRegisterMetaType<Action>("Session");
    qRegisterMetaType<Action>("Ticket");

    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
