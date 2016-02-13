#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QList>
#include <QPaintEvent>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
    QGraphicsScene * m_overlayScene;
public:
    explicit GraphicsScene(QObject *parent = 0);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void addItem(QGraphicsItem * item);
    virtual QGraphicsProxyWidget* addWidget(QWidget * item);
    void setOverlayScene(QGraphicsScene * scene);
    QGraphicsScene * overlayScene() const;
    void paintEvent(QPaintEvent *ev) ;

signals:

public slots:

private:
    QGraphicsRectItem* item1;
};

#endif // GRAPHICSSCENE_H
