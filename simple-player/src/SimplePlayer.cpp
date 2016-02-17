/*
* VLC-Qt Simple Player
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
*/

#include <QFileDialog>
#include <QInputDialog>
#include <QGroupBox>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>    // std::min


#include <VLCQtCore/Common.h>
#include <VLCQtCore/Stats.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtWidgets/WidgetVideo.h>

#include "SimplePlayer.h"
#include "ui_SimplePlayer.h"
#include "myvlcwidgetvideo.h"
#include "keyframe.h"

const int xVideoSize = 2000;

SimplePlayer::SimplePlayer(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::SimplePlayer),
      _media(0)
{
    ui->setupUi(this);

    _instance = new VlcInstance(VlcCommon::args(), this);
    _player = new VlcMediaPlayer(_instance);
    keyframes = new std::vector<Keyframe*>();

    wv = new MyVlcWidgetVideo(ui->graphicsView);
    wv->resize(xVideoSize,xVideoSize/2);
    wv->setMediaPlayer(_player);
    _player->setVideoWidget(wv);

    connect(wv, &MyVlcWidgetVideo::myMousePressedSignal, this, &SimplePlayer::videoClicked);

    targ1 = new QLabel(ui->graphicsView);
    targ1->setPixmap( QPixmap( "C:\\projects\\examples\\redSquare.png" ) );
    targ2 = new QLabel(ui->graphicsView);
    targ2->setPixmap( QPixmap( "C:\\projects\\examples\\redSquare.png" ) );
    setTargetPosition(200,200);

    ui->seek->setMediaPlayer(_player);

    connect(ui->actionOpenLocal, &QAction::triggered, this, &SimplePlayer::openLocal);
    connect(ui->actionPause, &QAction::toggled, _player, &VlcMediaPlayer::togglePause);
    connect(ui->actionStop, &QAction::triggered, _player, &VlcMediaPlayer::stop);
    connect(ui->openLocal, &QPushButton::clicked, this, &SimplePlayer::openLocal);
    connect(ui->loadKeyframeFile, &QPushButton::clicked, this, &SimplePlayer::readKeyframeFile);
    connect(ui->saveKeyframeFile, &QPushButton::clicked, this, &SimplePlayer::saveKeyframeFile);

    connect(ui->setKeyframe, &QPushButton::clicked, this, &SimplePlayer::setKeyframe);
    connect(ui->pause, &QPushButton::toggled, ui->actionPause, &QAction::toggle);
    connect(ui->stop, &QPushButton::clicked, _player, &VlcMediaPlayer::stop);

    connect(ui->actionOpenLocal, &QAction::triggered, this, &SimplePlayer::openLocal);

    connect(_player, &VlcMediaPlayer::timeChanged, this, &SimplePlayer::interpolateKeyframes);

    // debug - start video automatically
    _media = new VlcMedia("C:\\Users\\Drew\\Desktop\\Diving_with_Great_White_Shark_Part1.mp4", true, _instance);
    _player->open(_media);
    /*
    keyframes->push_back(new Keyframe(100,100,100));
    keyframes->push_back(new Keyframe(9000,300,300));
    keyframes->push_back(new Keyframe(15000,10,10));*/
}

SimplePlayer::~SimplePlayer()
{
    delete _player;
    delete _media;
    delete _instance;
    delete keyframes;
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

bool sortFn(Keyframe * lhs, Keyframe * rhs) { return lhs->frame < rhs->frame; }

void SimplePlayer::setKeyframe()
{
    int ms = _player->time(); // gets from in ms. Not v accurate. can interpolate, see http://stackoverflow.com/questions/11236432/how-do-i-get-libvlc-media-player-get-time-to-return-a-more-accurate-result
    qDebug(std::to_string(ms).c_str());
    if(!wv->lastPoint.isNull()) {
        keyframes->push_back(new Keyframe(ms,wv->lastPoint.x(),wv->lastPoint.y()));
        std::sort (keyframes->begin(), keyframes->end(), sortFn);
    }
}


bool bestToWrap(int leftX, int rightX) {
    int distOnPlane = abs(rightX - leftX);
    int distIfWraps = (xVideoSize - rightX) + leftX;
    return distIfWraps < distOnPlane;
}

int getXInterpWithWrap(Keyframe * last, Keyframe * next, double t){
    int leftX = std::min(last->x, next->x);
    int rightX = std::max(last->x, next->x);

    if(bestToWrap(leftX, rightX)) {
        int tVal = (int) ((xVideoSize - rightX + leftX) * t);
        if(leftX == last->x)
            return ((last->x - tVal) + xVideoSize) % xVideoSize;
        else
            return (last->x + tVal) % xVideoSize;
    } else {
        return ((rightX - leftX) * t) + leftX;
    }
}

void SimplePlayer::interpolateKeyframes(int t) {
    if(keyframes->size() < 2)
        return;

    Keyframe * last;
    Keyframe * next;

    for(int i = 1; i < keyframes->size(); i++){
        last = keyframes->at(i-1);
        next = keyframes->at(i);

        if(last->frame <= t && next->frame >= t) {
            // can interp - so get t
            int timescale = next->frame - last->frame;
            int timeAlong = t - last->frame;
            double t = (double) timeAlong / (double) timescale;

            int yInterp = ((next->y - last->y) * t) + last->y;
            int xInterp = getXInterpWithWrap(last, next, t);

            setTargetPosition(xInterp, yInterp);
        }
    }
}

void SimplePlayer::readKeyframeFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::homePath(), tr("Multimedia files(*)"));
    if (filename.isEmpty())
        return;

    std::ifstream file(filename.toStdString());

    while(file) {
        std::vector<std::string>   result;
        std::string                line;
        std::getline(file,line);

        std::stringstream          lineStream(line);
        std::string                cell;

        std::getline(lineStream,cell,',');
        std::string time = cell;
        std::getline(lineStream,cell,',');
        std::string x = cell;
        std::getline(lineStream,cell,',');
        std::string y = cell;

        if(time != "" && x != "" && y != "")
            keyframes->push_back(new Keyframe(std::stoi(time), std::stoi(x), std::stoi(y)));
    }
}

void SimplePlayer::saveKeyframeFile()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(), tr("CSV (*.csv)"));
    if (filename.isEmpty())
        return;

    std::ofstream SaveFile(filename.toStdString());

    for (int i = 0; i < keyframes->size(); i++) {
        Keyframe * kf = keyframes->at(i);
        SaveFile << kf->frame << "," << kf->x << "," << kf->y << std::endl;
    }

    SaveFile.close();
}

void SimplePlayer::videoClicked()
{
    QPoint point = wv->lastPoint;
    setTargetPosition(point.x(),point.y());
}

void SimplePlayer::setTargetPosition(int x, int y)
{
    targ1->setGeometry(QRect(x-50,y-3,100,6));
    targ2->setGeometry(QRect(x-3,y-50,6,100));
}
