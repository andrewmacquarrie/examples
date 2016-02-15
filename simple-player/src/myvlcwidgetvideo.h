#ifndef MYVLCWIDGETVIDEO_H
#define MYVLCWIDGETVIDEO_H


#include <VLCQtWidgets/WidgetVideo.h>
#include <QMouseEvent>

class MyVlcWidgetVideo : public VlcWidgetVideo
{
    Q_OBJECT

signals:
   void myMousePressedSignal();

public:
    explicit MyVlcWidgetVideo(QWidget* parent = 0);
    explicit MyVlcWidgetVideo(VlcMediaPlayer  * player, QWidget * parent);
   QPoint lastPoint;

protected:
    void mousePressEvent(QMouseEvent *me);

};

#endif // MYVLCWIDGETVIDEO_H
