QT += core gui widgets charts
LIBS += -lz

CONFIG += console c++11
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -ffunction-sections -fdata-sections
QMAKE_LFLAGS += -Wl,--gc-sections

TEMPLATE = app
TARGET = ct_geometry_test

INCLUDEPATH += ../source

HEADERS += ../source/data.h \
           ../source/data/ctGeometry.h \
           ../source/data/DICOM.h \
           ../source/data/planTiming.h \
           ../source/data/dose.h \
           ../source/data/egsphant.h \
           ../source/data/input.h \
           ../source/libraries/gzstream.h

SOURCES += ct_geometry_test.cpp \
           ../source/data.cpp \
           ../source/data/ctGeometry.cpp \
           ../source/data/DICOM.cpp \
           ../source/data/database.cpp \
           ../source/data/planTiming.cpp \
           ../source/data/dose.cpp \
           ../source/data/egsphant.cpp \
           ../source/data/input.cpp \
           ../source/libraries/gzstream.cpp
