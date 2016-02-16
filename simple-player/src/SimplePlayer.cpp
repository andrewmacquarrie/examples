/*
* VLC-Qt Simple Player
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
*/

#include <QFileDialog>
#include <QInputDialog>
#include <QGroupBox>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtWidgets/WidgetVideo.h>

#include "SimplePlayer.h"
#include "ui_SimplePlayer.h"
#include "myvlcwidgetvideo.h"

SimplePlayer::SimplePlayer(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::SimplePlayer),
      _media(0)
{
    qDebug( "C Style Debug Message" );

    ui->setupUi(this);

    _instance = new VlcInstance(VlcCommon::args(), this);
    _player = new VlcMediaPlayer(_instance);

    wv = new MyVlcWidgetVideo(ui->graphicsView);
    wv->resize(2000,1000);
    wv->setMediaPlayer(_player);
    _player->setVideoWidget(wv);

    connect(wv, &MyVlcWidgetVideo::myMousePressedSignal, this, &SimplePlayer::videoClicked);

    targ1 = new QLabel(ui->graphicsView);
    targ1->setPixmap( QPixmap( "C:\\projects\\examples\\redSquare.png" ) );
    targ2 = new QLabel(ui->graphicsView);
    targ2->setPixmap( QPixmap( "C:\\projects\\examples\\redSquare.png" ) );
    setTargetPosition(200,200);

    ui->volume->setMediaPlayer(_player);
    ui->volume->setVolume(50);
    ui->seek->setMediaPlayer(_player);

    connect(ui->actionOpenLocal, &QAction::triggered, this, &SimplePlayer::openLocal);
    connect(ui->actionPause, &QAction::toggled, _player, &VlcMediaPlayer::togglePause);
    connect(ui->actionStop, &QAction::triggered, _player, &VlcMediaPlayer::stop);
    connect(ui->openLocal, &QPushButton::clicked, this, &SimplePlayer::openLocal);
    connect(ui->pause, &QPushButton::toggled, ui->actionPause, &QAction::toggle);
    connect(ui->stop, &QPushButton::clicked, _player, &VlcMediaPlayer::stop);

    connect(ui->actionOpenLocal, &QAction::triggered, this, &SimplePlayer::openLocal);

    // debug - start video automatically
    _media = new VlcMedia("C:\\Users\\Drew\\Desktop\\Diving_with_Great_White_Shark_Part1.mp4", true, _instance);
    _player->open(_media);
}

SimplePlayer::~SimplePlayer()
{
    delete _player;
    delete _media;
    delete _instance;
    delete ui;
}

void SimplePlayer::openLocal()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::homePath(), tr("Multimedia files(*)"));
    if (file.isEmpty())
        return;

    _media = new VlcMedia(file, true, _instance);
    _player->open(_media);
}


void SimplePlayer::videoClicked()
{
    qDebug( "clicked" );
    QPoint point = wv->lastPoint;
    setTargetPosition(point.x(),point.y());
}

void SimplePlayer::setTargetPosition(int x, int y)
{
    targ1->setGeometry(QRect(x-50,y-3,100,6));
    targ2->setGeometry(QRect(x-3,y-50,6,100));
}
