// TITLE:   Optimization_Interface/include/graphics/drone_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for drone

#ifndef DRONE_GRAPHICS_ITEM_H_
#define DRONE_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "../models/drone_model_item.h"

namespace interface {

class DroneGraphicsItem : public QGraphicsItem {
 public:
    explicit DroneGraphicsItem(DroneModelItem *model,
                               QGraphicsItem *parent = nullptr,
                               qint32 size = 16);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
              QWidget *widget = nullptr) override;
    void expandScene();

 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                       const QVariant &value) override;
 private:
    void initialize();
    QPen pen_;
    QBrush brush_;
    DroneModelItem *model_;
    qint32 size_ = 32;
};

}  // namespace interface

#endif  // DRONE_GRAPHICS_ITEM_H_
