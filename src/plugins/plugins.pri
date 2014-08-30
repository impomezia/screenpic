TEMPLATE       = lib
CONFIG         += plugin static
lessThan(QT_MAJOR_VERSION, 5):!win32-msvc*:QMAKE_CXXFLAGS += -std=c++0x

DEPENDPATH += \
    . \
    ../../../ \
    3rdparty \
    editor \
    ../../../schat \

VPATH += \
    . \
    ../../../ \
    3rdparty \
    editor \
    ../../../schat \

INCLUDEPATH += \
    . \
    ../../../ \
    3rdparty \
    editor \
    ../../../schat \

CONFIG(debug, debug|release) { 
  RCC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/debug/rcc
  MOC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/debug/moc
  OBJECTS_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/debug/obj
  DESTDIR = ../../../../bin/debug
} else {
  RCC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/release/rcc
  MOC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/release/moc
  OBJECTS_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/release/obj
  DESTDIR = ../../../../bin
}
