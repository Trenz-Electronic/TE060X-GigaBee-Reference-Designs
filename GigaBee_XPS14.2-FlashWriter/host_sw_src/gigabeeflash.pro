#-------------------------------------------------
#
# Project created by QtCreator 2012-01-03T10:32:13
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = gigabeeflash
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    gfwupgrader.cpp \
    gfwdevicecommunicator.cpp

HEADERS += \
    gfwdevicecommunicator.h \
    gfwupgrader.h \
    my_types.h
