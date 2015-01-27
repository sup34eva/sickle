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

linter.name = linter
linter.input = SOURCES
linter.CONFIG += no_link no_clean combine
linter.commands = cpplint --root=$$PWD $$PWD/include/*.hpp $$PWD/src/*.cpp
linter.output = lint.log
QMAKE_EXTRA_COMPILERS += linter

lint.target = lint
lint.CONFIG = recursive
lint.recurse_target = lint.log
QMAKE_EXTRA_TARGETS += lint

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
