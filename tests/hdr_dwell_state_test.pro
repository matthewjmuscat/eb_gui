QT += core gui widgets charts
LIBS += -lz

CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app
TARGET = hdr_dwell_state_test

INCLUDEPATH += ../source

HEADERS += ../source/data.h \
           ../source/interface.h \
           ../source/data/ctGeometry.h \
           ../source/data/DICOM.h \
           ../source/data/planTiming.h \
           ../source/data/dose.h \
           ../source/data/egsphant.h \
           ../source/data/input.h \
           ../source/GUI/appInterface.h \
           ../source/GUI/doseInterface.h \
           ../source/GUI/ebInterface.h \
           ../source/GUI/phantInterface.h \
           ../source/GUI/sourceInterface.h \
           ../source/libraries/gzstream.h

SOURCES += hdr_dwell_state_test.cpp \
           ../source/data.cpp \
           ../source/interface.cpp \
           ../source/data/ctGeometry.cpp \
           ../source/data/database.cpp \
           ../source/data/DICOM.cpp \
           ../source/data/planTiming.cpp \
           ../source/data/dose.cpp \
           ../source/data/egsphant.cpp \
           ../source/data/input.cpp \
           ../source/GUI/appInterface.cpp \
           ../source/GUI/doseInterface.cpp \
           ../source/GUI/ebInterface.cpp \
           ../source/GUI/phantInterface.cpp \
           ../source/GUI/sourceInterface.cpp \
           ../source/libraries/gzstream.cpp