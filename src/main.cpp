// Copyright 2015 PsychoLama

#include <mainwindow.hpp>
#include <QApplication>
#include <QSurfaceFormat>
#include <QTranslator>
#include <QFile>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	QFile style(":/styles/valve.qss");
	style.open(QFile::ReadOnly);
	app.setStyleSheet(style.readAll());

	QTranslator translator;
	auto locale = QLocale::system().name();
	translator.load(QString(":/lang/editor_") + locale);
	app.installTranslator(&translator);

	MainWindow window;
	window.showMaximized();

	return app.exec();
}
