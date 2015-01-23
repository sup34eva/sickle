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
    scripts/build-docs.sh

RESOURCES += res/resources.qrc

FORMS += res/mainwindow.ui

TRANSLATIONS = res/editor_fr.ts

#DOXYFILE = Doxyfile
#COVERALLS = .coveralls.yml

#linter.name = linter
#linter.input = SOURCES
#linter.CONFIG += combine no_link no_clean
#linter.commands = $$PWD/cpplint.py --verbose=7 ${QMAKE_FILE_IN} >> ${QMAKE_FILE_OUT}
#linter.output = lint.log

#doxygen.name = doxygen
#doxygen.input = DOXYFILE
#doxygen.CONFIG += no_link no_clean
#doxygen.commands = ( cat ${QMAKE_FILE_IN} ; echo "INPUT = $$PWD/src \ $$PWD/include" ) | doxygen - >> ${QMAKE_FILE_OUT}
#doxygen.output = doxygen.log

#coverage.name = coverage
#coverage.input = COVERALLS
#coverage.CONFIG += no_link no_clean
#coverage.commands = coveralls -n -y ${QMAKE_FILE_IN} -r ${OBJECTS_DIR} -b $$PWD --dump ${QMAKE_FILE_OUT}
#coverage.output = coverage.json

#QMAKE_EXTRA_COMPILERS += linter doxygen coverage
#POST_TARGETDEPS += doxygen.log lint.log coverage.json
