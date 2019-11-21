// TITLE:   Optimization_Interface/include/graphics/plane_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation of plane constraint

#ifndef PLANE_GRAPHICS_ITEM_H_
#define PLANE_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "../models/plane_model_item.h"
#include "../graphics/polygon_resize_handle.h"

namespace interface {

qreal const PLANE_BORDER = 15;

class PlaneGraphicsItem : public QGraphicsItem {
 public:
    explicit PlaneGraphicsItem(PlaneModelItem *model,
                               QGraphicsItem *parent = nullptr);
    ~PlaneGraphicsItem();
    PlaneModelItem *model_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    int type() const override;

    void expandScene();
    void flipDirection();

 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    void initialize();
    QPen pen_;
    QBrush brush_;
    PolygonResizeHandle *p1_handle_;
    PolygonResizeHandle *p2_handle_;
};

}  // namespace interface

#endif  // PLANE_GRAPHICS_ITEM_H_
