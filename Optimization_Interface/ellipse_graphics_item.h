// TITLE:   Optimization_Interface/ellipse_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef ELLIPSE_GRAPHICS_ITEM_H
#define ELLIPSE_GRAPHICS_ITEM_H

#include <QGraphicsItem>
#include <QPainter>

#include "ellipse_model_item.h"
#include "ellipse_resize_handle.h"

namespace interface {

const qreal ELLIPSE_BORDER = 15;

class EllipseGraphicsItem : public QGraphicsItem
{
public:
    explicit EllipseGraphicsItem(EllipseModelItem *model, QGraphicsItem *parent = nullptr);
    ~EllipseGraphicsItem();
    EllipseModelItem *model_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    int type() const override;

    void expandScene();
    void flipConvex();
protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
private:
    void initialize();
    QPen pen_;
    QBrush brush_;
    EllipseResizeHandle *resize_handle_;
};

}  // namespace

#endif // ELLIPSE_GRAPHICS_ITEM_H
