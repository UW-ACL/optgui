// TITLE:   Optimization_Interface/point_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for point constraint

#ifndef POINT_GRAPHICS_ITEM_H_
#define POINT_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "point_model_item.h"

namespace interface {

const qreal POINT_BORDER = 20;
const qreal POINT_RADIUS = 0.05;

class PointGraphicsItem : public QGraphicsItem {
 public:
    explicit PointGraphicsItem(PointModelItem *model,
                                 QGraphicsItem *parent = nullptr);
    ~PointGraphicsItem();
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
    void initialize();
    QPen pen_;
    QBrush brush_;
};

}  // namespace interface

#endif  // POINT_GRAPHICS_ITEM_H_
