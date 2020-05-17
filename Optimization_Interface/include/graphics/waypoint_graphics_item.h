// TITLE:   Optimization_Interface/include/graphics/waypoint_graphics_item.h
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

// Graphical waypoint

#ifndef WAYPOINT_GRAPHICS_ITEM_H_
#define WAYPOINT_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/models/point_model_item.h"

namespace optgui {

class WaypointGraphicsItem : public QGraphicsItem {
 public:
    explicit WaypointGraphicsItem(PointModelItem *model,
                                 quint32 index,
                                 QGraphicsItem *parent = nullptr,
                                 qreal radius = 14);
    PointModelItem *model_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    int type() const override;
    void expandScene();
    void setIndex(quint32 index);

 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                       const QVariant &value) override;

 private:
    QPen pen_;
    QBrush brush_;
    qreal radius_;
    qreal getScalingFactor() const;
    quint32 index_;
};

}  // namespace optgui

#endif  // WAYPOINT_GRAPHICS_ITEM_H_
