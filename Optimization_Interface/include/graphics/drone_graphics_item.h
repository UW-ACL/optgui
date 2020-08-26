// TITLE:   Optimization_Interface/include/graphics/drone_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for drone

#ifndef DRONE_GRAPHICS_ITEM_H_
#define DRONE_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/globals.h"
#include "include/models/drone_model_item.h"

namespace optgui {

class DroneGraphicsItem : public QGraphicsItem {
 public:
    explicit DroneGraphicsItem(DroneModelItem *model,
                               QGraphicsItem *parent = nullptr,
                               qreal size = 20);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
              QWidget *widget = nullptr) override;
    int type() const override;

    DroneModelItem *model_;

 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                       const QVariant &value) override;

 private:
    QPen pen_;
    QBrush brush_;
    qreal size_;
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // DRONE_GRAPHICS_ITEM_H_
