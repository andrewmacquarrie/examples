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
    EqualizerDialog.cpp \
    graphicsscene.cpp \
    overlaidgraphicsview.cpp \
    myvlcwidgetvideo.cpp

HEADERS    += SimplePlayer.h \
    EqualizerDialog.h \
    graphicsscene.h \
    overlaidgraphicsview.h \
    myvlcwidgetvideo.h

FORMS      += SimplePlayer.ui \
    EqualizerDialog.ui

LIBS       += -lVLCQtCored -lVLCQtWidgetsd


# Edit below for custom library location
LIBS       += -L/Software/VLC-Qt_1.0.1_win64_msvc2013/lib -lVLCQtCored -lVLCQtWidgetsd
INCLUDEPATH += /Software/VLC-Qt_1.0.1_win64_msvc2013/include
