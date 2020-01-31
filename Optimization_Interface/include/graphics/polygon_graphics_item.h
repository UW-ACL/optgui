// TITLE:   Optimization_Interface/include/graphics/polygon_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation of polygon constraint

#ifndef POLYGON_GRAPHICS_ITEM_H_
#define POLYGON_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/models/polygon_model_item.h"
#include "include/graphics/polygon_resize_handle.h"

namespace interface {

qreal const POLYGON_BORDER = 15;

class PolygonGraphicsItem : public QGraphicsItem {
 public:
    explicit PolygonGraphicsItem(PolygonModelItem *model,
                                 QGraphicsItem *parent = nullptr);
    ~PolygonGraphicsItem();
    PolygonModelItem *model_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0) override;
    int type() const override;

    void expandScene();
    void flipDirection();

 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    void initialize();
    bool isConvex() const;
    QPen pen_;
    QBrush brush_;
    QVector<PolygonResizeHandle *> *resize_handles_;
    qreal getScalingFactor() const;
};

}  // namespace interface

#endif  // POLYGON_GRAPHICS_ITEM_H_
