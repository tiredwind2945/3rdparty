# source code for this simple demo
SOURCES = contourplot.cpp  \
    contourplotanimator.cpp

# configure Qt
CONFIG += link_prl qt
QT += core gui xml svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

# output executable name
TARGET = contourplot

# include JKQTPlotter source code
DEPENDPATH += ../../lib  ../../qmake/staticlib/jkqtplotterlib
INCLUDEPATH += ../../lib
CONFIG (debug, debug|release) {
    LIBS += -L../../qmake/staticlib/jkqtplotterlib/debug -ljkqtplotterlib_debug
} else {
    LIBS += -L../../qmake/staticlib/jkqtplotterlib/release -ljkqtplotterlib
}
message("LIBS = $$LIBS")

HEADERS += \
    contourplotanimator.h

win32-msvc*: DEFINES += _USE_MATH_DEFINES
win32-msvc*: DEFINES += NOMINMAX




