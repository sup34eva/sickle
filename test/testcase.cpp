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
	//auto cube = view->addChild<Cube>();
	auto after = ui->viewport->findChildren<Geometry>().length();
	QCOMPARE(after, before + 1);
	//Q_CHECK_PTR(cube);
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

void TestCase::cleanup() {
	ui->viewport->clearLevel();
	//win.hide();
}

QTEST_MAIN(TestCase)
