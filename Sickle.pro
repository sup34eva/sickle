QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sickle
TEMPLATE = app

RC_FILE = res/sickle.rc

include(Files.pri)

SOURCES += \
    src/main.cpp
