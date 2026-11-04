QT += core

CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app
TARGET = source_calibration_test

SOURCES += source_calibration_test.cpp \
           ../source/data/sourceCalibration.cpp

HEADERS += ../source/data/sourceCalibration.h