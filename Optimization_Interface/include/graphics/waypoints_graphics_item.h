// TITLE:   Optimization_Interface/include/graphics/waypoints_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation of waypoint

#ifndef WAYPOINTS_GRAPHICS_ITEM_H_
#define WAYPOINTS_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>
#include <QPointF>
#include <QVector>

#include "polygon_resize_handle.h"
#include "../models/path_model_item.h"

namespace interface {

class WaypointsGraphicsItem : public QGraphicsItem {
 public:
    explicit WaypointsGraphicsItem(PathModelItem *model,
                                   QGraphicsItem *parent = nullptr,
                                   qreal size = 16);
    ~WaypointsGraphicsItem();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    void expandScene();
    void removeHandle(PolygonResizeHandle *handle);
    int type() const override;

 protected:
    QPainterPath shape() const override;
//    QVariant itemChange(GraphicsItemChange change,
//                        const QVariant &value) override;

 private:
    void initialize();
    PathModelItem *model_;
    QVector<PolygonResizeHandle *> *resize_handles_;
    qreal line_width_;
    qreal size_;
    qreal getScalingFactor();
};

}  // namespace interface

#endif  // WAYPOINTS_GRAPHICS_ITEM_H_
