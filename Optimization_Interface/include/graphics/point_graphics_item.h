// TITLE:   Optimization_Interface/include/graphics/point_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for point constraint

#ifndef POINT_GRAPHICS_ITEM_H_
#define POINT_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "../models/point_model_item.h"

namespace interface {

const qreal POINT_BORDER = 3;

class PointGraphicsItem : public QGraphicsItem {
 public:
    explicit PointGraphicsItem(PointModelItem *model,
                               QGraphicsItem *parent = nullptr,
                               quint32 size = 4);
    ~PointGraphicsItem();
    PointModelItem *model_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    int type() const override;

    void expandScene();
    void setMarker(uint32_t);

 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    void initialize();
    QPen pen_;
    QBrush brush_;
    uint32_t marker_;
    quint32 radius_;
};

}  // namespace interface

#endif  // POINT_GRAPHICS_ITEM_H_
