// TITLE:   Optimization_Interface/polygon_resize_handle.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical resize handle

#ifndef POLYGON_RESIZE_HANDLE_H_
#define POLYGON_RESIZE_HANDLE_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

namespace interface {

const qreal HANDLE_SIZE = 16;

class PolygonResizeHandle : public QGraphicsEllipseItem {
 public:
    explicit PolygonResizeHandle(QPointF *point,
                                 QGraphicsItem *parent);
    void updatePos();
    void updateModel(QPointF diff);
    int type() const override;
    QPointF *getPoint();
    void setColor(const QColor color);
 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
 private:
    void expandScene();
    QPointF *point_;
    bool resize_;
};

}  // namespace interface

#endif  // POLYGON_RESIZE_HANDLE_H_
