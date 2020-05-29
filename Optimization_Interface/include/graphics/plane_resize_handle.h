// TITLE:   Optimization_Interface/include/graphics/plane_resize_handle.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical resize handle

#ifndef PLANE_RESIZE_HANDLE_H_
#define PLANE_RESIZE_HANDLE_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

#include "include/models/plane_model_item.h"

namespace optgui {

class PlaneResizeHandle : public QGraphicsEllipseItem {
 public:
    explicit PlaneResizeHandle(PlaneModelItem *model,
                               bool isP2,
                               QGraphicsItem *parent,
                               qreal size = 14);
    void updatePos();
    void updateModel(QPointF diff);
    int type() const override;
    QPointF getPoint();
    void setColor(const QColor color);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    void expandScene();
    PlaneModelItem *model_;
    bool isP2_;
    bool resize_;
    qreal size_;
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // PLANE_RESIZE_HANDLE_H_
