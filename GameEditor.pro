#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T20:52:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameEditor
TEMPLATE = app

ENV_CC = $$(CC)
if(!isEmpty(ENV_CC)) {
    QMAKE_CC = $$(CC)
}

ENV_CXX = $$(CXX)
if(!isEmpty(ENV_CXX)) {
    QMAKE_CXX = $$(CXX)
    QMAKE_LINK = $$(CXX)
}

INCLUDEPATH += include

CONFIG += c++14 rtti static
QMAKE_LIBDIR += $$(LIBDIR)

SOURCES += \
    src/main.cpp\
    src/camera.cpp \
    src/geometry.cpp \
    src/viewport.cpp \
    src/group.cpp \
    src/mainwindow.cpp \
    src/cube.cpp

HEADERS  += \
    include/camera.hpp \
    include/cube.hpp \
    include/geometry.hpp \
    include/globals.hpp \
    include/group.hpp \
    include/mainwindow.hpp \
    include/viewport.hpp

DISTFILES += \
    res/lit.frag \
    res/lit.vert \
    res/unlit.frag \
    res/unlit.vert \
    README.md \
    .travis.yml \
    scripts/install-mingw32.sh \
    Doxyfile \
    scripts/build-docs.sh \
    .clang-format \
    .gitattributes \
    .gitignore \
    .gitmodules \
    .coveralls.yml \
    CodeStyle.xml \
    res/style.qss

RESOURCES += res/resources.qrc

FORMS += res/mainwindow.ui

TRANSLATIONS = res/editor_fr.ts

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
