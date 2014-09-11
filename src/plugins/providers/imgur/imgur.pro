TARGET = ImgurProviderPlugin
QT = core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS = \
    ImgurProviderPlugin.h \
    ImgurUploader.h \
    ImgurSettings.h \

SOURCES = \
    ImgurProviderPlugin.cpp \
    ImgurUploader.cpp \
    ImgurSettings.cpp \
 
include(../../plugins.pri)
