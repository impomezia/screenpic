
DEPENDPATH += \
    . \
    3rdparty \
    editor \
    schat \

VPATH += \
    . \
    3rdparty \
    editor \
    schat \

INCLUDEPATH += \
    . \
    3rdparty \
    editor \
    schat \

!win32:VERSION = 2.2.0

CONFIG(debug, debug|release) { 
  RCC_DIR = ../tmp/$${TEMPLATE}/$${TARGET}/debug/rcc
  MOC_DIR = ../tmp/$${TEMPLATE}/$${TARGET}/debug/moc
  OBJECTS_DIR = ../tmp/$${TEMPLATE}/$${TARGET}/debug/obj
  LIBS += -L../bin/debug
  DESTDIR = ../bin/debug
} else { 
  RCC_DIR = ../tmp/$${TEMPLATE}/$${TARGET}/release/rcc
  MOC_DIR = ../tmp/$${TEMPLATE}/$${TARGET}/release/moc
  OBJECTS_DIR = ../tmp/$${TEMPLATE}/$${TARGET}/release/obj
  LIBS += -L../bin
  DESTDIR = ../bin
}

win32-msvc*:DEFINES += _CRT_RAND_S

