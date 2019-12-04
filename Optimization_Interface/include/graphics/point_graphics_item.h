// TITLE:   Optimization_Interface/include/graphics/point_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for point constraint

#ifndef POINT_GRAPHICS_ITEM_H_
#define POINT_GRAPHICS_ITEM_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

#include "../models/point_model_item.h"

namespace interface {

qreal const POINT_SIZE = 14;

class PointGraphicsItem : public QGraphicsEllipseItem {
 public:
    explicit PointGraphicsItem(PointModelItem *model,
                               QGraphicsItem *parent = nullptr,
                               qreal size = POINT_SIZE);
    PointModelItem *model_;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    int type() const override;
    void expandScene();

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    bool resize_;
    qreal radius_;
    qreal getScalingFactor() const;
};

}  // namespace interface

#endif  // POINT_GRAPHICS_ITEM_H_
