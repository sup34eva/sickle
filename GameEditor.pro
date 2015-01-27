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
