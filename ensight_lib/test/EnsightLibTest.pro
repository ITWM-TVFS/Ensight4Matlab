QT += core testlib
QT -= gui

TARGET = EnsightLibTest

# Include path to Eigen library: must contain the directory "Eigen/Dense"
INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/include/eigen3

INCLUDEPATH *= ../../../libs/3dparty/eigen_v3_2_1

# Include path to Ensight lib
INCLUDEPATH += ../include
LIBS *= -L../lib -lEnsightLib


DESTDIR = build
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
