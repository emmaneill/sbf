TEMPLATE = lib
TARGET = sbfcommonhandler

include (../../build.pri)

HEADERS += \
    sbfCommonHandler.h \
    sbfCommonHandlerPrivate.h
SOURCES += \
    sbfCommonHandler.c
LIBS += \
    -lsbfcore \
    -lsbftransport

target.path = $$top_build/lib
INSTALLS += target
