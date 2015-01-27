#ifndef TESTCASE_HPP
#define TESTCASE_HPP

#include <QtTest/QtTest>
#include <viewport.hpp>

class TestCase : public QObject
{
	Q_OBJECT
private slots:
	void init();
	void childAddTest();
	void renderBenchmark();
	void cleanup();
private:
	Viewport view;
};

#endif // TESTCASE_HPP
