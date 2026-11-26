#include "widget.h"

#include <QApplication>
#include <QMessageBox>
#include <exception>
#include <cstdlib>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try {
        Widget w;
        w.show();
        return QApplication::exec();
    } catch (const std::exception &error) {
        QMessageBox::critical(nullptr, "No se pudo iniciar el juego",
                              QString::fromUtf8(error.what())
                                  + "\nRevisa los recursos y vuelve a compilar el proyecto.");
        return EXIT_FAILURE;
    }
}
