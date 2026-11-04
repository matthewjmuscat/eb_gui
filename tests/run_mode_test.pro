QT += core

CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app
TARGET = run_mode_test

SOURCES += run_mode_test.cpp \
           ../source/data/runMode.cpp

HEADERS += ../source/data/runMode.h