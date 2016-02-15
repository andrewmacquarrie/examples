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

#include "EqualizerDialog.h"

#include "SimplePlayer.h"
#include "ui_SimplePlayer.h"
#include "graphicsscene.h"
#include "myvlcwidgetvideo.h"

SimplePlayer::SimplePlayer(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::SimplePlayer),
      _media(0),
      _equalizerDialog(new EqualizerDialog(this))
{
    qDebug( "C Style Debug Message" );

    ui->setupUi(this);

    _instance = new VlcInstance(VlcCommon::args(), this);
    _player = new VlcMediaPlayer(_instance);
    _equalizerDialog->setMediaPlayer(_player);
    //_player->setVideoWidget(ui->video);

    //GraphicsScene * scene = new GraphicsScene();

    // move the video into the graphics view by setting it as it's parents. Works, but doesn't seem to be part of scene so z-index doesnt overlay rect
    //ui->video->setParent(ui->graphicsView);

    // create a rect
    //QGraphicsRectItem * item1 = new QGraphicsRectItem(0,0,100,100);
    //item1->setBrush(QBrush(Qt::red));
    //scene->addItem(item1);
    //item1->setZValue(1000);

    wv = new MyVlcWidgetVideo(ui->graphicsView);
    //wv->setFrameRect(QRect(20,20,100,50));
    wv->resize(2000,1000);
    wv->setMediaPlayer(_player);
    _player->setVideoWidget(wv);
    //scene->addWidget(wv);

    /*
    QFrame * fr = new QFrame(ui->graphicsView);
    fr->resize(2000,1000);
    fr->setStyleSheet("background-color:black;");
    QGraphicsView * ogv = new QGraphicsView(ui->graphicsView);
    ogv->setStyleSheet("background-color:white;");
    QGraphicsScene * ogs = new QGraphicsScene();
    QGraphicsRectItem * itm = ogs->addRect(400, 400, 100, 300, Qt::SolidLine, Qt::SolidPattern); // strut
    itm->setRect(100,100,30,30);
    itm->setBrush(QBrush(QColor(255, 0, 0, 128)));
    ogv->setScene(ogs);
    ogv->setFrameRect(QRect(0,0,1000,1000));
    ogv->setSceneRect(0,0,500,500); // this one isnt transparent, it's white
    ogv->setStyleSheet("background: transparent"); // when over a QFrame, this also works. However over a VlcWidgetVideo it doesn't
    ogs->setBackgroundBrush(QBrush(QColor(0, 0, 0, 0))); // this does seem to make the scene transparent!
    */

    connect(wv, &MyVlcWidgetVideo::myMousePressedSignal, this, &SimplePlayer::videoClicked);


    targ1 = new QLabel(ui->graphicsView);
    targ1->setPixmap( QPixmap( "C:\\projects\\examples\\redSquare.png" ) );
    targ2 = new QLabel(ui->graphicsView);
    targ2->setPixmap( QPixmap( "C:\\projects\\examples\\redSquare.png" ) );
    setTargetPosition(200,200);

    //ui->video->setMediaPlayer(_player);
    ui->volume->setMediaPlayer(_player);
    ui->volume->setVolume(50);
    ui->seek->setMediaPlayer(_player);

    connect(ui->actionOpenLocal, &QAction::triggered, this, &SimplePlayer::openLocal);
    connect(ui->actionOpenUrl, &QAction::triggered, this, &SimplePlayer::openUrl);
    connect(ui->actionPause, &QAction::toggled, _player, &VlcMediaPlayer::togglePause);
    connect(ui->actionStop, &QAction::triggered, _player, &VlcMediaPlayer::stop);
    connect(ui->openLocal, &QPushButton::clicked, this, &SimplePlayer::openLocal);
    connect(ui->openUrl, &QPushButton::clicked, this, &SimplePlayer::openUrl);
    connect(ui->pause, &QPushButton::toggled, ui->actionPause, &QAction::toggle);
    connect(ui->stop, &QPushButton::clicked, _player, &VlcMediaPlayer::stop);
    connect(ui->equalizer, &QPushButton::clicked, _equalizerDialog, &EqualizerDialog::show);

    connect(ui->actionOpenLocal, &QAction::triggered, this, &SimplePlayer::openLocal);


    // debug - start video automatically
    _media = new VlcMedia("C:\\Users\\Drew\\Desktop\\Diving_with_Great_White_Shark_Part1.mp4", true, _instance);
    _player->open(_media);

    //notification = new QGraphicsScene();
    //notification->addRect(0, 0, 10, 10, Qt::SolidLine, Qt::SolidPattern); // strut
   // ui->graphicsView->setOverlayScene(notification);
    //scene->setOverlayScene(notification);

    //ui->graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
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
    QString file =
            QFileDialog::getOpenFileName(this, tr("Open file"),
                                         QDir::homePath(),
                                         tr("Multimedia files(*)"));

    if (file.isEmpty())
        return;

    _media = new VlcMedia(file, true, _instance);

    _player->open(_media);
}

void SimplePlayer::openUrl()
{
    QString url =
            QInputDialog::getText(this, tr("Open Url"), tr("Enter the URL you want to play"));

    if (url.isEmpty())
        return;

    _media = new VlcMedia(url, _instance);

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
