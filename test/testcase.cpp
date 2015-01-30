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
	auto item = ui->infoWidget->topLevelItem(row);
	auto cont = ui->infoWidget->itemWidget(item, 1)->layout();
	for(int i = 0; i < 3; i++) {
		auto spin = static_cast<QDoubleSpinBox*>(cont->itemAt(i)->widget());
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

void TestCase::viewMove() {
	auto cam = ui->viewport->camera();
	auto oldView = cam->view();

	auto list = {
		Qt::Key_Up,
		Qt::Key_Down,
		Qt::Key_Right,
		Qt::Key_Left,
		Qt::Key_PageUp,
		Qt::Key_PageDown,
		Qt::Key_Z,
		Qt::Key_S,
		Qt::Key_Q,
		Qt::Key_D
	};

	for(auto key : list)
		QTest::keyPress(ui->viewport, key);

	QCOMPARE(cam->view(), oldView);
}

void TestCase::infobox() {
	auto position = QVector3D(1, 1, 1);
	ui->viewport->camera()->position(position);
	QCOMPARE(ui->camPos->text(), QString("X: %1, Y: %2, Z: %3").arg(position.x()).arg(position.y()).arg(position.z()));
}

void TestCase::cleanup() {
	ui->viewport->clearLevel();
}

QTEST_MAIN(TestCase)
