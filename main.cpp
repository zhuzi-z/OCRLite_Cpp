#include "main_window.h"
#include <QApplication>
#include <QStyle>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setQuitOnLastWindowClosed(false);
    Main_Window w;
    QObject::connect(&app, &QApplication::screenAdded, &w, &Main_Window::screens_changed_ac);
    QObject::connect(&app, &QApplication::screenRemoved, &w, &Main_Window::screens_changed_ac);
    w.show();
    return app.exec();
}
