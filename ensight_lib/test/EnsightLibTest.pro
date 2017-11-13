QT += core testlib
QT -= gui

TARGET = EnsightLibTest

# Include path to Ensight lib
INCLUDEPATH += ../include
LIBS *= -L../lib -lEnsightLib

# Configuration file for the include directories
!include(../EnsightLibConfig.pri) {
    error("Cannot find include file '../EnsightLibConfig.pri'")
}

#DESTDIR = build
OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc
RCC_DIR = build/.rcc

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    bboxtests.cpp \
    ensightconstanttests.cpp \
    ensightvariabletests.cpp \
    ensightreadertests.cpp \
    main.cpp

HEADERS += \
    bboxtests.h \
    ensightconstanttests.h \
    ensightvariabletests.h \
    ensightreadertests.h
