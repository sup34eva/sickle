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
    README.md \
    .travis.yml \
    Doxyfile \
    scripts/build-docs.sh \
    .clang-format \
    .gitattributes \
    .gitignore \
    .gitmodules \
    .coveralls.yml \
    CodeStyle.xml \
    Files.pri \
    CPPLINT.cfg \
    res/sickle.ico \
    res/sickle.rc

RESOURCES += res/resources.qrc

FORMS += res/mainwindow.ui

TRANSLATIONS = res/editor_fr.ts

linter.name = linter
linter.input = SOURCES
linter.CONFIG += no_link no_clean combine
linter.commands = cpplint --root=$$PWD $$PWD/include/*.hpp $$PWD/src/*.cpp
linter.output = lint
QMAKE_EXTRA_COMPILERS += linter

lint.target = lint
lint.CONFIG = recursive
lint.recurse_target = lint
QMAKE_EXTRA_TARGETS += lint

unix|win*-g* {
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++
}
