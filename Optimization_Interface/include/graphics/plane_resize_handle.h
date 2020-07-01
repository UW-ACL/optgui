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
    // update graphic with model info
    void updatePos();
    // update model info
    void updateModel(QPointF diff);

    // unique graphic type
    int type() const override;

    // get pos of handle
    QPointF getPoint();

    // draw graphic
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

 protected:
    // detect mouse input
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    // data model
    PlaneModelItem *model_;

    // type of handle: P1, P2
    bool isP2_;

    // flag for using handle
    bool resize_;

    // size of handle
    qreal size_;

    // scale zoom level
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // PLANE_RESIZE_HANDLE_H_
