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
#include "/Software/boost_1_59_0/boost/math/quaternion.hpp"

#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/angle/degrees.hpp>

const int xVideoSize = 1400;

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
    connect(ui->exportTracking, &QPushButton::clicked, this, &SimplePlayer::exportTrackingCSV);

    connect(ui->setKeyframe, &QPushButton::clicked, this, &SimplePlayer::setKeyframe);
    connect(ui->setJumpCut, &QPushButton::clicked, this, &SimplePlayer::setJumpCut);
    connect(ui->pause, &QPushButton::toggled, ui->actionPause, &QAction::toggle);
    connect(ui->stop, &QPushButton::clicked, _player, &VlcMediaPlayer::stop);

    connect(ui->actionOpenLocal, &QAction::triggered, this, &SimplePlayer::openLocal);

    connect(_player, &VlcMediaPlayer::timeChanged, this, &SimplePlayer::interpolateKeyframes);

    // debug - start video automatically
    _media = new VlcMedia("E:\\Videos\\RealMemories4096x2048.mp4", true, _instance);
    _player->open(_media);

    /*
    keyframes->push_back(new Keyframe(5,1000,400));
    keyframes->push_back(new Keyframe(9000,1000,600));*/
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

    if(!wv->lastPoint.isNull()) {
        keyframes->push_back(new Keyframe(ms,wv->lastPoint.x(),wv->lastPoint.y()));
        std::sort (keyframes->begin(), keyframes->end(), sortFn);
    }
}

void SimplePlayer::setJumpCut()
{
    int ms = _player->time(); // gets from in ms. Not v accurate. can interpolate, see http://stackoverflow.com/questions/11236432/how-do-i-get-libvlc-media-player-get-time-to-return-a-more-accurate-result

    if(!wv->lastPoint.isNull()) {
        if(keyframes->size() > 1) {
            Keyframe * lastKey = keyframes->back();
            keyframes->push_back(new Keyframe(ms-1,lastKey->x,lastKey->y));
        }
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
        return ((next->x - last->x) * t) + last->x;
    }
}

boost::math::quaternion<float> getAsQuaternion(int xPos, int yPos) {
    // need to convert these to -180 -> 180 // -90 -> 90 in radians
    const float degToRad = 57.295779513f;
    const float rho = 1.0f; // not sure what this is. const 1 in example. May be magnitude? all results zero if set to zero
    float lon = ((float) xPos / (float) xVideoSize) * 360.0f - 180.0f ;
    float lat = ((float) yPos / (float) (xVideoSize / 2)) * 180.0f - 90.0f;

    lon = lon * 0.5f; // why?
    lat = lat * 0.5f; // why? - but seems to produce correct rotation in omiplayerOmni?

    float phi = 0.0f; // not sure what this is. Roll?

    boost::math::quaternion<float> yaw = boost::math::spherical(rho, 0.0f, lon / degToRad, phi);
    boost::math::quaternion<float> pitch = boost::math::spherical(rho, lat / degToRad, 0.0f, phi);


    return pitch * yaw;
}

bool SimplePlayer::setNextAndLastFromTime(int t, Keyframe ** last, Keyframe ** next) {
    for(int i = 1; i < keyframes->size(); i++){
        *last = keyframes->at(i-1);
        *next = keyframes->at(i);

        if(keyframes->at(i-1)->frame <= t && keyframes->at(i)->frame >= t) {
            return true; // success, kf found
        }
    }

    return false; // cant find kf
}

void getInterpolation(int time, Keyframe * last, Keyframe * next, int * x, int * y) {
    int timescale = next->frame - last->frame;
    int timeAlong = time - last->frame;
    double t = (double) timeAlong / (double) timescale;

    *y = ((next->y - last->y) * t) + last->y;
    *x = getXInterpWithWrap(last, next, t);
}

void SimplePlayer::interpolateKeyframes(int time) {
    if(keyframes->size() < 2)
        return;

    Keyframe * last;
    Keyframe * next;

    if(setNextAndLastFromTime(time, &last, &next)) {
        int yInterp, xInterp;
        getInterpolation(time, last, next, &xInterp, &yInterp);
        setTargetPosition(xInterp, yInterp);
    }
}

void SimplePlayer::exportTrackingCSV()
{
    if(keyframes->size() < 2)
        return;

    QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(), tr("CSV (*.csv)"));
    if (filename.isEmpty())
        return;

    int fps = std::stoi(ui->fpsEntry->text().toStdString());
    float ms = (1.0f / (float) fps) * 1000; // s -> ms

    std::ofstream SaveFile(filename.toStdString());

    for(float time = 1; time < _player->length(); time = time + ms){
        Keyframe * last;
        Keyframe * next;

        if(setNextAndLastFromTime(time, &last, &next)) {
            int yInterp, xInterp;
            getInterpolation(time, last, next, &xInterp, &yInterp);
            boost::math::quaternion<float> q = getAsQuaternion(xInterp, yInterp);
            // m_w(q.R_component_1()), m_x(q.R_component_2()), m_y(q.R_component_3()), m_z(q.R_component_4()) : acording to https://sourceforge.net/p/xengine/code/HEAD/tree/trunk/XEngine/include/XEngine/Math/XQuaternion.inl
            SaveFile << q.R_component_2() << "," << q.R_component_3() << "," << q.R_component_4() << "," << q.R_component_1() * -1.0f << std::endl;
        }
    }
    SaveFile.close();
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
