QT += core
QT -= gui

TARGET = write_file
TEMPLATE = app

# Include path to Ensight lib
INCLUDEPATH += ../../include
LIBS += -L../../lib/ -lEnsightLib

# Configuration file for the include directories
!include(../../EnsightLibConfig.pri) {
    error("Cannot find include file '../../EnsightLibConfig.pri'")
}

unix {
    QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic
}

SOURCES += main.cpp
