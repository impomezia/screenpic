TARGET = StatisticsPlugin
QT = core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS = \
    StatisticsPlugin.h \
    StatisticsUploader.h \
    Statistics.h

SOURCES = \
    StatisticsPlugin.cpp \
    StatisticsUploader.cpp
 
include(../../plugins.pri)
