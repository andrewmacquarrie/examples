#include "myvlcwidgetvideo.h"

#include "myvlcwidgetvideo.h"


MyVlcWidgetVideo::MyVlcWidgetVideo(QWidget* parent) :
    VlcWidgetVideo(parent) {}

MyVlcWidgetVideo::MyVlcWidgetVideo(VlcMediaPlayer  * player, QWidget * parent) :
    VlcWidgetVideo(player, parent) {}


void MyVlcWidgetVideo::mousePressEvent(QMouseEvent *event)
 {
     if (event->button() == Qt::LeftButton) {
         lastPoint = event->pos();
     }
     emit(myMousePressedSignal());
 }
