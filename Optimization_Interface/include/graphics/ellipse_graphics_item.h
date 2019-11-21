// TITLE:   Optimization_Interface/include/graphics/ellipse_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for ellipse constraint

#ifndef ELLIPSE_GRAPHICS_ITEM_H_
#define ELLIPSE_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "../models/ellipse_model_item.h"
#include "../graphics/ellipse_resize_handle.h"

namespace interface {

qreal const ELLIPSE_BORDER = 15;

class EllipseGraphicsItem : public QGraphicsItem {
 public:
    explicit EllipseGraphicsItem(EllipseModelItem *model,
                                 QGraphicsItem *parent = nullptr);
    ~EllipseGraphicsItem();
    EllipseModelItem *model_;

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
    EllipseResizeHandle *resize_handle_;
};

}  // namespace interface

#endif  // ELLIPSE_GRAPHICS_ITEM_H_
