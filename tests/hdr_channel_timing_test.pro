QT += core gui

CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app
TARGET = hdr_channel_timing_test

INCLUDEPATH += ../source

HEADERS += ../source/data/planTiming.h

SOURCES += hdr_channel_timing_test.cpp \
           ../source/data/planTiming.cpp