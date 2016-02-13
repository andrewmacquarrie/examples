#ifndef OVERLAIDGRAPHICSVIEW_H
#define OVERLAIDGRAPHICSVIEW_H

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QWidget>
#include <QGraphicsView>

class OverlaidGraphicsView : public QGraphicsView
{
    Q_OBJECT
    QGraphicsScene * m_overlayScene;
public:
    explicit OverlaidGraphicsView(QWidget* parent = 0);
    explicit OverlaidGraphicsView(QGraphicsScene * scene = 0, QWidget * parent = 0);
    void setOverlayScene(QGraphicsScene * scene);
    void drawForeground(QPainter * painter, const QRectF & rect);
    QGraphicsScene * overlayScene() const;
    void paintEvent(QPaintEvent *ev) ;
    virtual void paintOverlay();
    Q_SLOT void overlayChanged();

};

#endif // OVERLAIDGRAPHICSVIEW_H
