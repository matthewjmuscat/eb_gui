QT += core gui

CONFIG += console c++11
CONFIG -= app_bundle

TEMPLATE = app
TARGET = dicom_nested_sequence_test

INCLUDEPATH += ../source

HEADERS += ../source/data/DICOM.h

SOURCES += dicom_nested_sequence_test.cpp \
           ../source/data/DICOM.cpp \
           ../source/data/database.cpp