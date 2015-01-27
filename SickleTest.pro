QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SickleTest
TEMPLATE = app

CONFIG += testcase

include(Files.pri)

HEADERS += \
    test/testcase.hpp

SOURCES += \
    test/testcase.cpp

unix {
	QMAKE_LFLAGS += -static-libgcc -static-libstdc++ --coverage
	QMAKE_CXXFLAGS += --coverage

	gcov.name = gcov
	gcov.input = SOURCES
	gcov.CONFIG += no_link no_clean
	gcov.commands = gcov ${QMAKE_FILE_IN}.gcno
	gcov.output = coverage

	QMAKE_EXTRA_COMPILERS += gcov
	POST_TARGETDEPS += coverage
}