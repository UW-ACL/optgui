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

class PointGraphicsItem : public QGraphicsItem {
 public:
    explicit PointGraphicsItem(PointModelItem *model,
                               QGraphicsItem *parent = nullptr,
                               qreal radius = 14);
    // data model
    PointModelItem *model_;

    // rough area of graphic
    QRectF boundingRect() const override;
    // draw graphic
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    // unique type of graphic class
    int type() const override;

 protected:
    // shape to draw
    QPainterPath shape() const override;
    // update model when graphic is changed
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    // size of graphic
    qreal radius_;

    // graphical info
    QPen pen_;
    QBrush brush_;

    // scale zoom level
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // POINT_GRAPHICS_ITEM_H_
