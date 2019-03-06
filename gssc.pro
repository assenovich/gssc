QT += core
QT -= gui

TARGET = gssc
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++14

TEMPLATE = app

SOURCES += emitter.cpp
SOURCES += dispatcher.cpp
SOURCES += main.cpp

HEADERS += emitter.h
HEADERS += dispatcher.h
