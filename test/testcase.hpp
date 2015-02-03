#ifndef TESTCASE_HPP
#define TESTCASE_HPP

#include <QtTest/QtTest>
#include <mainwindow.hpp>
#include <viewport.hpp>

class TestCase : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void cleanup();
	// Editor core
	void renderBenchmark();
	void viewMove_data();
	void viewMove();
	void infobox();
	void newZone();
	void saveLoad();
	// Object creator
	void addCube();
	// Object editor
	void moveObject();
	void scaleObject();
	void createGroup();
	void moveGroup();
	void paintFace();
	// Zone linker
	void linkZones();
private:
	void moveVector(int row);
	MainWindow win;
	Ui::MainWindow* ui;
};

#endif // TESTCASE_HPP
