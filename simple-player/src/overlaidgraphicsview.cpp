#include "overlaidgraphicsview.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QWidget>
#include <QGraphicsView>

OverlaidGraphicsView::OverlaidGraphicsView(QWidget* parent) :
    QGraphicsView(parent), m_overlayScene(NULL) {}

OverlaidGraphicsView::OverlaidGraphicsView(QGraphicsScene * scene, QWidget * parent) :
    QGraphicsView(scene, parent), m_overlayScene(NULL) {}

void OverlaidGraphicsView::setOverlayScene(QGraphicsScene * scene) {
    if (scene == m_overlayScene) return;
    m_overlayScene = scene;
    connect(scene, SIGNAL(changed(QList<QRectF>)), SLOT(overlayChanged()));
    update();
}

QGraphicsScene * OverlaidGraphicsView::overlayScene() const { return m_overlayScene; }

void OverlaidGraphicsView::paintEvent(QPaintEvent *ev) {
    QGraphicsView::paintEvent(ev);
    if (m_overlayScene) paintOverlay();
}
void OverlaidGraphicsView::paintOverlay() {
    QPainter p(viewport());
    p.setRenderHints(renderHints());
    m_overlayScene->render(&p, viewport()->rect());
}
void OverlaidGraphicsView::overlayChanged() { update(); }

void OverlaidGraphicsView::drawForeground(QPainter * painter, const QRectF & rect) {
    QGraphicsView::drawForeground(painter, rect);
    if (m_overlayScene) paintOverlay();
}
