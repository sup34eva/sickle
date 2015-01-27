#include <mainwindow.hpp>
#include <QApplication>
#include <QSurfaceFormat>
#include <QTranslator>
#include <QFile>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QFile style(":/styles/dark.qss");
	style.open(QFile::ReadOnly);
	app.setStyleSheet(style.readAll());

	QTranslator translator;
	QString locale = QLocale::system().name();
	translator.load(QString(":/lang/editor_") + locale);
	app.installTranslator(&translator);

	MainWindow window;
	window.resize(800, 600);
	window.showMaximized();

	return app.exec();
}
