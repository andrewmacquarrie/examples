/*
* VLC-Qt Simple Player
* Copyright (C) 2015 Tadej Novak <tadej@tano.si>
*/

#ifndef SIMPLEPLAYER_H_
#define SIMPLEPLAYER_H_

#include <QMainWindow>
#include <QGraphicsView>
#include <QLabel>
#include <myvlcwidgetvideo.h>

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
    void videoClicked();

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

    void setTargetPosition(int x, int y);
};

#endif // SIMPLEPLAYER_H_
