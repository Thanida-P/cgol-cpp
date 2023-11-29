#include "cgol.hpp"
#include "parser_utils.hpp"
#include "gui.hpp"

#include <QApplication>
#include <QtWidgets>
#include <QMainWindow>
#include <QTimer>
#include <QGraphicsView>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow m;

    return app.exec();
}
