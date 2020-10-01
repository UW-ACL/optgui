// TITLE:   Optimization_Interface/include/graphics/ellipse_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for ellipse constraint

#ifndef ELLIPSE_GRAPHICS_ITEM_H_
#define ELLIPSE_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/models/ellipse_model_item.h"
#include "include/graphics/ellipse_resize_handle.h"

namespace optgui {

qreal const ELLIPSE_BORDER = 15;

class EllipseGraphicsItem : public QGraphicsItem {
 public:
    explicit EllipseGraphicsItem(EllipseModelItem *model,
                                 QGraphicsItem *parent = nullptr);
    ~EllipseGraphicsItem();

    // data model
    EllipseModelItem *model_;

    // rough area of graphic
    QRectF boundingRect() const override;

    // draw graphic
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    // unique type for graphic class
    int type() const override;

    // flip direction of constraint (not supported in socp)
    void flipDirection();

    // change color to red for displaying errors
    void setRed(bool isOverlap);

 protected:
    // shape to paint
    QPainterPath shape() const override;

    // update model when graphic is changed
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    QPen pen_;
    QPen clearance_pen_;
    QBrush brush_;

    // handles for resizing
    EllipseResizeHandle *width_handle_;
    EllipseResizeHandle *height_handle_;
    EllipseResizeHandle *radius_handle_;

    // scale zoom level
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // ELLIPSE_GRAPHICS_ITEM_H_
