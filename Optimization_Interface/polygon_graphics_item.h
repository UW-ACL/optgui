// TITLE:   Optimization_Interface/polygon_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef POLYGON_GRAPHICS_ITEM_H
#define POLYGON_GRAPHICS_ITEM_H

#include <QGraphicsItem>
#include <QPainter>

#include "polygon_model_item.h"
#include "polygon_resize_handle.h"

namespace interface {

const qreal POLYGON_BORDER = 15;

class PolygonGraphicsItem : public QGraphicsItem
{
public:
    explicit PolygonGraphicsItem(PolygonModelItem *model, QGraphicsItem *parent = nullptr);
    ~PolygonGraphicsItem();
    PolygonModelItem *model_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
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
    QVector<PolygonResizeHandle *> *resize_handles_;
};

}  // namespace

#endif // POLYGON_GRAPHICS_ITEM_H
