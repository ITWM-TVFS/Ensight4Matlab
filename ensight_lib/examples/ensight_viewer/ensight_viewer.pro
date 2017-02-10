QT += core gui opengl

TARGET = ensight_viewer
TEMPLATE = app

# Include path to Eigen library: must contain the directory "Eigen/Dense"
INCLUDEPATH += /usr/local/include

# Include path to Ensight lib
INCLUDEPATH += ../../include

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
