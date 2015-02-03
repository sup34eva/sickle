#include "testcase.hpp"
#include <mainwindow.hpp>
#include <QDoubleSpinBox>
#include "./ui_mainwindow.h"

void TestCase::initTestCase() {
	ui = win.getUI();
	win.show();
}

void TestCase::addCube() {
	auto before = ui->viewport->findChildren<Geometry>().length();
	ui->newGeo->trigger();
	auto after = ui->viewport->findChildren<Geometry>().length();
	QCOMPARE(after, before + 1);
}

void TestCase::renderBenchmark() {
	for(int i = 0, num = 1000; i < num; i++) {
		auto child = ui->viewport->addChild<Cube>();
		child->position(QVector3D(i, i, i));
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

	auto index = ui->actorList->indexAt(QPoint(5, 5));
	ui->actorList->setCurrentIndex(index);

	moveVector(1);

	QCOMPARE(cube->position(), QVector3D(2, 2, 2));
}

void TestCase::scaleObject() {
	auto cube = ui->viewport->addChild<Cube>();

	auto index = ui->actorList->indexAt(QPoint(5, 5));
	ui->actorList->setCurrentIndex(index);

	moveVector(3);

	QCOMPARE(cube->scale(), QVector3D(2, 2, 2));
}

void TestCase::saveLoad() {
	ui->viewport->addChild<Cube>();
	ui->viewport->save("test.wld");
	ui->viewport->clearLevel();
	ui->viewport->load("test.wld");
	QCOMPARE(ui->viewport->findChildren<Cube>().length(), 1);
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
	QVERIFY(cam->view() != view);
}

void TestCase::infobox() {
	auto position = QVector3D(1, 1, 1);
	ui->viewport->camera()->position(position);
	QCOMPARE(ui->camPos->text(), QString("X: %1, Y: %2, Z: %3").arg(position.x()).arg(position.y()).arg(position.z()));
}

void TestCase::newZone() {
}

void TestCase::createGroup() {
}

void TestCase::moveGroup() {
}

void TestCase::paintFace() {
}

void TestCase::linkZones() {
}


void TestCase::cleanup() {
	ui->viewport->clearLevel();
}

QTEST_MAIN(TestCase)
