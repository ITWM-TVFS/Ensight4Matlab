QT += core
QT -= gui

TARGET = write_file
TEMPLATE = app

# Include path to Eigen library: must contain the directory "Eigen/Dense"
INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/include/eigen3

# Include path to Ensight lib
INCLUDEPATH += ../../include

unix {
    QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic
}

LIBS += -L../../lib/ -lEnsightLib

SOURCES += main.cpp
