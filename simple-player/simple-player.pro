#
# VLC-Qt Simple Player
# Copyright (C) 2015 Tadej Novak <tadej@tano.si>
#

TEMPLATE = subdirs

SUBDIRS += src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Users/Drew/Downloads/VLC-Qt_1.0.1_win64_msvc2013/lib/ -lVLCQtCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Users/Drew/Downloads/VLC-Qt_1.0.1_win64_msvc2013/lib/ -lVLCQtCore

INCLUDEPATH += $$PWD/../../../Users/Drew/Downloads/VLC-Qt_1.0.1_win64_msvc2013/include
DEPENDPATH += $$PWD/../../../Users/Drew/Downloads/VLC-Qt_1.0.1_win64_msvc2013/include

INCLUDEPATH += $$_PRO_FILE_PWD_
