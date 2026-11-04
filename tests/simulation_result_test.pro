QT += core

CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app
TARGET = simulation_result_test

SOURCES += simulation_result_test.cpp \
           ../source/data/simulationResult.cpp

HEADERS += ../source/data/simulationResult.h