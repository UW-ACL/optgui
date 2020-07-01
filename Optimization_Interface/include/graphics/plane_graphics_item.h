// TITLE:   Optimization_Interface/include/graphics/plane_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation of plane constraint

#ifndef PLANE_GRAPHICS_ITEM_H_
#define PLANE_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/models/plane_model_item.h"
#include "include/graphics/plane_resize_handle.h"

namespace optgui {

qreal const PLANE_BORDER = 15;

class PlaneGraphicsItem : public QGraphicsItem {
 public:
    explicit PlaneGraphicsItem(PlaneModelItem *model,
                               QGraphicsItem *parent = nullptr);
    ~PlaneGraphicsItem();

    // data model
    PlaneModelItem *model_;

    // rough area of graphic
    QRectF boundingRect() const override;
    // draw shape
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    // unique type of graphic
    int type() const override;

    // flip direction of graphic
    void flipDirection();

 protected:
    // shape to draw
    QPainterPath shape() const override;
    // update model when graphic is changed
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    // graphical info
    QPen pen_;
    QBrush brush_;

    // resize handles
    PlaneResizeHandle *p1_handle_;
    PlaneResizeHandle *p2_handle_;

    // scale zoom level
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // PLANE_GRAPHICS_ITEM_H_
