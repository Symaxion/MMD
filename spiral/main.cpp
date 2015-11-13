/*
 * Author: Frank Erens
 */

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include "MainWindow.hpp"

int main(int argc, char** argv) {
    QApplication app{argc, argv};

    MainWindow win{};
    win.show();

    return app.exec();
}
