#include "mainwindow.h"
#include "testmain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    //MainWindow window;
    TestMain window;
    window.resize(640, 480);
    window.show();


    return app.exec();
}
