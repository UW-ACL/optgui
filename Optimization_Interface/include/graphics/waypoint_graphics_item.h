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
    // data model
    PointModelItem *model_;

    // rough area of graphic
    QRectF boundingRect() const override;
    // draw graphic
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    // unique type for graphic
    int type() const override;
    // set ordering of waypoint
    void setIndex(quint32 index);

 protected:
    // shape to draw
    QPainterPath shape() const override;
    // update model when graphic changes
    QVariant itemChange(GraphicsItemChange change,
                       const QVariant &value) override;

 private:
    // graphical info
    QPen pen_;
    QBrush brush_;

    // size of graphic
    qreal radius_;

    // scale zoom level
    qreal getScalingFactor() const;

    // ordering of waypoint
    quint32 index_;
};

}  // namespace optgui

#endif  // WAYPOINT_GRAPHICS_ITEM_H_
