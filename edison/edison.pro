TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    edge/BgEdge.cpp \
    edge/BgEdgeDetect.cpp \
    edge/BgEdgeList.cpp \
    edge/BgGlobalFc.cpp \
    edge/BgImage.cpp \
    prompt/edison.cpp \
    prompt/globalFnc.cpp \
    prompt/libppm.cpp \
    prompt/parser.cpp \
    segm/ms.cpp \
    segm/msImageProcessor.cpp \
    segm/msSysPrompt.cpp \
    segm/RAList.cpp \
    segm/rlist.cpp

HEADERS += \
    edge/BgDefaults.h \
    edge/BgEdge.h \
    edge/BgEdgeDetect.h \
    edge/BgEdgeList.h \
    edge/BgImage.h \
    prompt/defs.h \
    prompt/edison.h \
    prompt/error.h \
    prompt/flags.h \
    prompt/imfilt.h \
    prompt/libppm.h \
    prompt/parser.h \
    segm/ms.h \
    segm/msImageProcessor.h \
    segm/msSys.h \
    segm/RAList.h \
    segm/rlist.h \
    segm/tdef.h
