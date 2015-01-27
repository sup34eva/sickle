#include "testcase.hpp"
#include <mainwindow.hpp>

void TestCase::init() {
	view.show();
}

void TestCase::childAddTest() {
	auto before = view.findChildren<Geometry>().length();
	view.addChild();
	auto after = view.findChildren<Geometry>().length();
	QCOMPARE(after, before + 1);
}

void TestCase::renderBenchmark() {
	for(int i = 0, num = 1000; i < num; i++) {
		auto child = view.addChild();
		child->position(QVector3D(i, i, i));
	}

	QBENCHMARK {
		view.updateNow();
	}
}

void TestCase::cleanup() {
	view.clearLevel();
	view.hide();
}

QTEST_MAIN(TestCase)
