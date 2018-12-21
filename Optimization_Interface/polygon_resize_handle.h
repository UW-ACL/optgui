// TITLE:   Optimization_Interface/polygon_resize_handle.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef POLYGON_RESIZE_HANDLE_H
#define POLYGON_RESIZE_HANDLE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

namespace interface {

const qreal POLYGON_HANDLE_SIZE = 16;

class PolygonResizeHandle : public QGraphicsEllipseItem
{
public:
    PolygonResizeHandle(QPointF *point, QGraphicsItem *parent);
    void updatePos();
    void updateModel(QPointF diff);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
private:
    void expandScene();
    QPointF *point_;
    bool resize_;
};

}  // namespace

#endif // POLYGON_RESIZE_HANDLE_H
