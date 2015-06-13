#include "testcase.hpp"
#include <cube.hpp>
#include <sphere.hpp>
#include <group.hpp>
#include <trigger.hpp>
#include <mainwindow.hpp>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QSignalSpy>
#include "./ui_mainwindow.h"

void TestCase::initTestCase() {
	ui = win.getUI();
	win.show();
	QTest::qWaitForWindowExposed(&win);
}

void TestCase::addCube() {
	auto before = ui->viewport->findChildren<Cube>().length() + 1;
	ui->newCube->trigger();
	auto after = ui->viewport->findChildren<Cube>().length();
	QCOMPARE(after, before);
}

void TestCase::renderBenchmark() {
	for(int i = 0, num = 10; i < num; i++)
		for(int j = 0; j < num; j++)
			for(int k = 0; k < num; k++) {
				auto child = ui->viewport->addChild<Sphere>();
				child->position(QVector3D(i, j, k));
			}

	QBENCHMARK {
		ui->viewport->updateNow();
	}
}

void TestCase::moveVector(int row) {
	for(int i = 0; i < 3; i++) {
		auto item = ui->infoWidget->topLevelItem(row)->child(i);
		auto spin = static_cast<QDoubleSpinBox*>(ui->infoWidget->itemWidget(item, 1));
		spin->setValue(2.0);
	}
}

void TestCase::moveObject() {
	auto cube = ui->viewport->addChild<Cube>();

	auto index = ui->actorList->indexAt(QPoint(15, 15));
	ui->actorList->setCurrentIndex(index);

	moveVector(1);

	QCOMPARE(cube->position(), QVector3D(2, 2, 2));
}

void TestCase::scaleObject() {
	auto cube = ui->viewport->addChild<Cube>();

	auto index = ui->actorList->indexAt(QPoint(15, 15));
	ui->actorList->setCurrentIndex(index);

	moveVector(3);

	QCOMPARE(cube->scale(), QVector3D(2, 2, 2));
}

void TestCase::saveLoad() {
	DefaultFileLoader loader;
	ui->viewport->addChild<Cube>();

	auto before = ui->viewport->findChildren<Cube>().length();
	loader.save(ui->viewport, "test.wld");

	cleanup();

	loader.load(ui->viewport, "test.wld");

	QCOMPARE(ui->viewport->findChildren<Cube>().length(), before);
}

Q_DECLARE_METATYPE(Qt::Key)

void TestCase::viewMove_data() {
	QTest::addColumn<Qt::Key>("key");

	QTest::newRow("Up") << Qt::Key_Up;
	QTest::newRow("Down") << Qt::Key_Down;
	QTest::newRow("Right") << Qt::Key_Right;
	QTest::newRow("Left") << Qt::Key_Left;
	QTest::newRow("PageUp") << Qt::Key_PageUp;
	QTest::newRow("PageDown") << Qt::Key_PageDown;
	QTest::newRow("Z") << Qt::Key_Z;
	QTest::newRow("S") << Qt::Key_S;
	QTest::newRow("Q") << Qt::Key_Q;
	QTest::newRow("D") << Qt::Key_D;
}

void TestCase::viewMove() {
	auto cam = ui->viewport->camera();
	auto view = cam->view();
	QFETCH(Qt::Key, key);
	QTest::keyPress(ui->viewport, key);
	QVERIFY2(cam->view() != view, "Key event didn't change the camera view");
}

void TestCase::infobox() {
	auto position = QVector3D(1, 1, 1);
	ui->viewport->camera()->position(position);
	QCOMPARE(ui->camPos->text(), QString("X: %1, Y: %2, Z: %3").arg(position.x()).arg(position.y()).arg(position.z()));
}

void TestCase::newZone() {
	auto count = ui->viewport->world()->zoneList().size() + 1;
	ui->tabBar->setCurrentIndex(ui->tabBar->count() - 1);
	QCOMPARE(ui->viewport->world()->zoneList().size(), count);
}

void TestCase::createGroup() {
	QList<Cube*> cubes;
	for(int i = 0; i < 10; i++) {
		cubes.append(ui->viewport->addChild<Cube>());
	}

	ui->actorList->selectAll();

	auto group = ui->viewport->addChild<Group>();
	foreach(auto cube, cubes) {
		QCOMPARE(cube->parent(), group);
	}
}

void TestCase::moveGroup() {
	QList<Cube*> cubes;
	for(int i = 0; i < 10; i++) {
		cubes.append(ui->viewport->addChild<Cube>());
	}

	ui->actorList->selectAll();
	auto group = ui->viewport->addChild<Group>();
	ui->actorList->clearSelection();

	auto index = ui->actorList->indexAt(QPoint(10, 10));
	ui->actorList->setCurrentIndex(index);

	moveVector(1);

	QCOMPARE(group->position(), QVector3D(2, 2, 2));
}

void TestCase::paintFace() {
	auto cube = ui->viewport->addChild<Cube>();

	auto index = ui->actorList->indexAt(QPoint(5, 5));
	ui->actorList->setCurrentIndex(index);

	auto item = ui->infoWidget->topLevelItem(4)->child(0);
	auto btn = static_cast<QPushButton*>(ui->infoWidget->itemWidget(item, 1));
	btn->click();

	auto color = QColor(2, 2, 2);
	foreach (auto widget, QApplication::topLevelWidgets()) {
		if (auto dialog = qobject_cast<QColorDialog*>(widget)) {
			dialog->setCurrentColor(color);
		}
	}

	QCOMPARE(qvariant_cast<QColor>(cube->colors().at(0)), color);
}

void TestCase::linkZones() {
	auto count = ui->viewport->findChildren<Trigger>().size() + 1;
	ui->newTrigger->trigger();
	QCOMPARE(ui->viewport->findChildren<Trigger>().size(), count);
}

void TestCase::cleanup() {
	ui->actionNew->trigger();
	QTest::waitForEvents();
}

QTEST_MAIN(TestCase)
