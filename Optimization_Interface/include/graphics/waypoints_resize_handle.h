// TITLE:   Optimization_Interface/include/graphics/waypoints_resize_handle.h
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

// Graphical resize handle

#ifndef WAYPOINTS_RESIZE_HANDLE_H_
#define WAYPOINTS_RESIZE_HANDLE_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

#include "include/models/path_model_item.h"

namespace optgui {

class WaypointsResizeHandle : public QGraphicsEllipseItem {
 public:
    explicit WaypointsResizeHandle(PathModelItem *model,
                                 quint32 index,
                                 QGraphicsItem *parent,
                                 qreal size = 14);
    void updatePos();
    void updateModel(QPointF diff);
    int type() const override;
    QPointF getPoint();
    void setColor(const QColor color);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    quint32 index_;

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    void expandScene();
    PathModelItem *model_;
    bool resize_;
    qreal size_;
    qreal getScalingFactor();
};

}  // namespace optgui

#endif  // WAYPOINTS_RESIZE_HANDLE_H_
