#-------------------------------------------------
#
# Project created by QtCreator 2014-12-16T20:52:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GameEditor
TEMPLATE = app

include(Files.pri)

SOURCES += \
    src/main.cpp

unix|win*-g* {
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++
}
