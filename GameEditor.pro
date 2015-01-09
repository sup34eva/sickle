#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T20:52:49
#
#-------------------------------------------------

QT       += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameEditor
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    camera.cpp \
    geometry.cpp \
    viewport.cpp \
    group.cpp \
    testmain.cpp

HEADERS  += \
    mainwindow.h \
    camera.h \
    geometry.h \
    viewport.h \
    group.h \
    globals.h \
    testmain.h

DISTFILES += \
    lit.frag \
    lit.vert \
    unlit.frag \
    unlit.vert \
    README.md

RESOURCES += resources.qrc

FORMS += testmain.ui
