#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Configuración de la aplicación
    app.setApplicationName("Compilador Tools");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("CompilerTools");
    
    // Crear y mostrar ventana principal
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}