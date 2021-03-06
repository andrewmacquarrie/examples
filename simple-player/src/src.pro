#
# VLC-Qt Simple Player
# Copyright (C) 2015 Tadej Novak <tadej@tano.si>
#

# TARGET      = simple-player
TEMPLATE    = app
CONFIG 	   += c++11

QT         += widgets

SOURCES    += main.cpp \
    SimplePlayer.cpp \
    myvlcwidgetvideo.cpp \
    keyframe.cpp

HEADERS    += SimplePlayer.h \
    myvlcwidgetvideo.h \
    keyframe.h

FORMS      += SimplePlayer.ui

LIBS       += -lVLCQtCored -lVLCQtWidgetsd


# Edit below for custom library location
LIBS       += -L/Software/VLC-Qt_1.0.1_win64_msvc2013/lib -lVLCQtCored -lVLCQtWidgetsd
INCLUDEPATH += /Software/VLC-Qt_1.0.1_win64_msvc2013/include

INCLUDEPATH += /Software/boost_1_59_0/
