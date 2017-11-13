QT += core gui opengl

TARGET = ensight_viewer
TEMPLATE = app

# Include path to Ensight lib
INCLUDEPATH += ../../include

# Configuration file for the include directories
!include(../../EnsightLibConfig.pri) {
    error("Cannot find include file '../../EnsightLibConfig.pri'")
}

unix {
    QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic
}

LIBS += -lGLU
LIBS += -L../../lib/ -lEnsightLib

SOURCES += main.cpp\
           mainwidget.cpp \
           viewer.cpp \
           glwidget.cpp

HEADERS += mainwidget.h \
           viewer.h \
           glwidget.h

FORMS += mainwidget.ui \
         viewer.ui
