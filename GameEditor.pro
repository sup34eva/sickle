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

unix|win*-g++ {
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++
}

SOURCES += src/main.cpp\
    src/camera.cpp \
    src/geometry.cpp \
    src/viewport.cpp \
    src/group.cpp \
    src/testmain.cpp

HEADERS  += \
    include/camera.h \
    include/geometry.h \
    include/viewport.h \
    include/group.h \
    include/globals.h \
    include/testmain.h

DISTFILES += \
    res/lit.frag \
    res/lit.vert \
    res/unlit.frag \
    res/unlit.vert \
    README.md \
    .travis.yml \
    scripts/install-mingw32.sh \
    res/wireframe.vert \
    res/wireframe.frag

RESOURCES += res/resources.qrc

FORMS += res/testmain.ui

linter.name = linter
linter.input = SOURCES
linter.CONFIG += combine no_link no_clean target_predeps
linter.commands = $$PWD/cpplint.py --verbose=7 ${QMAKE_FILE_IN} >> ${QMAKE_FILE_OUT}
linter.output = ${QMAKE_VAR_OBJECTS_DIR}lint.txt
QMAKE_EXTRA_COMPILERS += linter
