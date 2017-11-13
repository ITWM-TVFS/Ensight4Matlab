TEMPLATE = lib
VERSION = 1.0

QT *= core
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4) {
    CONFIG(debug, debug|release){
        TARGET = EnsightLib5d
    }
    CONFIG(release, debug|release){
        TARGET = EnsightLib5
    }
} else {
    CONFIG(debug, debug|release){
        TARGET = EnsightLibd
    }
    CONFIG(release, debug|release){
        TARGET = EnsightLib
    }
}

# Configuration file for the include directories
!include(EnsightLibConfig.pri) {
    error("Cannot find include file 'EnsightLibConfig.pri'")
}


DESTDIR = lib
OBJECTS_DIR = lib/.obj
MOC_DIR = lib/.moc
RCC_DIR = lib/.rcc
UI_DIR = lib/.ui

unix {
    # CONFIG += staticlib
    QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic
}

win32{
    CONFIG   += shared static
    #CONFIG  += dll
}

DESTDIR = lib
OBJECTS_DIR = lib/.obj
MOC_DIR = lib/.moc
RCC_DIR = lib/.rcc
UI_DIR = lib/.ui



SOURCES += \
    src/ensightlib.cpp \
    src/ensightpart.cpp \
    src/ensightwriter.cpp \
    src/ensightasciiwriter.cpp \
    src/ensightbinarywriter.cpp \
    src/ensightobj.cpp \
    src/ensightreader.cpp \
    src/ensightasciireader.cpp \
    src/ensightbinaryreader.cpp \
    src/bbox.cpp \
    src/ensightcell.cpp \
    src/ensightvariable.cpp \
    src/ensightconstant.cpp \
    src/ensightdef.cpp \
    src/ensightbarycentriccoordinates.cpp \
    src/ensightsubdivtreeimpl.cpp

HEADERS += \
    include/ensightlib.h \
    include/ensightpart.h \
    include/ensightwriter.h \
    include/ensightvariable.h \
    include/ensightconstant.h \
    include/ensightasciiwriter.h \
    include/ensightbinarywriter.h \
    include/ensightobj.h \
    include/ensightreader.h \
    include/ensightasciireader.h \
    include/ensightbinaryreader.h \
    include/ensightdef.h \
    include/ensightcell.h \
    include/bbox.h \
    include/ensightbarycentriccoordinates.h \
    include/eigentypes.h \
    include/ensightsubdivtree.h \
    include/ensightsubdivtreeimpl.h
