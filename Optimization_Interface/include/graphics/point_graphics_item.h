// TITLE:   Optimization_Interface/include/graphics/point_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for point constraint

#ifndef POINT_GRAPHICS_ITEM_H_
#define POINT_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/models/point_model_item.h"

namespace optgui {

qreal const POINT_SIZE = 14;

class PointGraphicsItem : public QGraphicsItem {
 public:
    explicit PointGraphicsItem(PointModelItem *model,
                               QGraphicsItem *parent = nullptr,
                               qreal size = POINT_SIZE);
    PointModelItem *model_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    int type() const override;
    void expandScene();

 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    qreal radius_;
    QPen pen_;
    QBrush brush_;
    qreal getScalingFactor() const;
    void initialize();
};

}  // namespace optgui

#endif  // POINT_GRAPHICS_ITEM_H_
