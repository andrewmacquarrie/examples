/*
* VLC-Qt Simple Player
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
*/

#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>

#include <VLCQtCore/Common.h>

#include "SimplePlayer.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("VLC-Qt Simple Player");
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

    QApplication a(argc, argv);
    VlcCommon::setPluginPath(a.applicationDirPath() + "/plugins");

    SimplePlayer* mainWindow = new SimplePlayer();
    mainWindow->show();

    return a.exec();
}
