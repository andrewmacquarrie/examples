/*
* VLC-Qt Simple Player
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
*/

#ifndef SIMPLEPLAYER_H_
#define SIMPLEPLAYER_H_

#include <vector>
#include <QMainWindow>
#include <QGraphicsView>
#include <QLabel>
#include <myvlcwidgetvideo.h>
#include "keyframe.h"

namespace Ui {
    class SimplePlayer;
}

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class SimplePlayer : public QMainWindow
{
    Q_OBJECT
public:
    explicit SimplePlayer(QWidget *parent = 0);
    ~SimplePlayer();

private slots:
    void openLocal();
    void setKeyframe();
    void setJumpCut();
    void videoClicked();
    void interpolateKeyframes(int t);
    void readKeyframeFile();
    void saveKeyframeFile();
    void exportTrackingCSV();
    void updateTable();
    void gotoKeyframe();
    void deleteKeyframe();
    void backwardsFrame();
    void forwardsFrame();
    void tableValueChanged(int row, int col);

private:
    Ui::SimplePlayer *ui;
    QGraphicsView *graphicsView;
    QGraphicsScene *notification;

    VlcInstance *_instance;
    VlcMedia *_media;
    VlcMediaPlayer *_player;

    MyVlcWidgetVideo * wv;

    QLabel * targ1;
    QLabel * targ2;

    bool ignoreCellChanges;

    std::vector<Keyframe*> * keyframes;

    void setTargetPosition(int x, int y);
    bool setNextAndLastFromTime(int t, Keyframe ** last, Keyframe ** next);
    Keyframe * getFrameBeforeTime(int t);
};

#endif // SIMPLEPLAYER_H_
