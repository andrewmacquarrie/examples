#include "graphicsscene.h"
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include "qmath.h"

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    this->setBackgroundBrush(Qt::gray);

    item1 = new QGraphicsRectItem(0,0,120,120);
    item1->setBrush(QBrush(Qt::red));
    this->addItem(item1);
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    qDebug() << Q_FUNC_INFO << mouseEvent->scenePos();

    QPointF p = mouseEvent->scenePos();
    item1->setPos(p);

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphicsScene::addItem(QGraphicsItem * item)
{
    // keep marker above all other items.
    QGraphicsScene::addItem(item);
    item1->setZValue(item->zValue() + 1);
}

QGraphicsProxyWidget* GraphicsScene::addWidget(QWidget * widget)
{
    // keep marker above all other items.
    QGraphicsProxyWidget * proxy = QGraphicsScene::addWidget(widget);
    qDebug() << proxy->zValue();

    item1->setZValue(proxy->zValue() + 1);
    return proxy;
}

void GraphicsScene::setOverlayScene(QGraphicsScene * scene) {
    if (scene == m_overlayScene) return;
    m_overlayScene = scene;
    //connect(scene, SIGNAL(changed(QList<QRectF>)), SLOT(overlayChanged()));
    update();
}
