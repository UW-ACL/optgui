// TITLE:   Optimization_Interface/drone_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for drone

#include <QGraphicsItem>
#include <QPainter>

#ifndef DRONE_GRAPHICS_ITEM_H_
#define DRONE_GRAPHICS_ITEM_H_

namespace interface {

class DroneGraphicsItem : public QGraphicsItem {
 public:
    explicit DroneGraphicsItem(QPointF *model,
                               QGraphicsItem *parent = nullptr);
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
    QPointF *model_;
};

}  // namespace interface

#endif  // DRONE_GRAPHICS_ITEM_H_
