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

CONFIG += c++11 rtti
QMAKE_LFLAGS += -static-libgcc -static-libstdc++
QMAKE_LIBDIR += $$(LIBDIR)

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
    scripts/install-mingw32.sh

RESOURCES += res/resources.qrc

FORMS += res/testmain.ui

lintD.depends += compiler_rcc_make_all
lintD.depends += compiler_moc_header_make_all
lintD.depends += compiler_uic_make_all
lintD.commands = $(CXX) $(CXXFLAGS) -fsyntax-only $(INCPATH) $(SOURCES)

equals($$(PLATFORM), "win32")|win32|win64 {
    lint.commands = $(MAKE) -f $(MAKEFILE).Debug lintD
    QMAKE_EXTRA_TARGETS += lint
    Debug:QMAKE_EXTRA_TARGETS += lintD
} else {
    lint = lintD
    QMAKE_EXTRA_TARGETS += lint
}
