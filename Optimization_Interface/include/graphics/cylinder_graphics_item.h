// TITLE:   Optimization_Interface/include/graphics/cylinder_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for cylinder hoop constraint

#ifndef CYLINDER_GRAPHICS_ITEM_H_
#define CYLINDER_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/models/cylinder_model_item.h"
#include "include/graphics/cylinder_resize_handle.h"

namespace optgui {

qreal const CYLINDER_BORDER = 15;

class CylinderGraphicsItem : public QGraphicsItem {
 public:
    explicit CylinderGraphicsItem(CylinderModelItem *model,
                                 quint32 index,
                                 QGraphicsItem *parent = nullptr);
    ~CylinderGraphicsItem();

    // data model
    CylinderModelItem *model_;

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

    // set ordering of cylinder
    void setIndex(quint32 index);

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
    CylinderResizeHandle *width_handle_;
    CylinderResizeHandle *height_handle_;
    CylinderResizeHandle *trigger_width_handle_;

    // scale zoom level
    qreal getScalingFactor() const;

    // ordering of cylinder
    quint32 index_;
};

}  // namespace optgui

#endif  // CYLINDER_GRAPHICS_ITEM_H_
