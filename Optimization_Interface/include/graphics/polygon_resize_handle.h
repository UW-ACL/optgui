// TITLE:   Optimization_Interface/include/graphics/polygon_resize_handle.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical resize handle

#ifndef POLYGON_RESIZE_HANDLE_H_
#define POLYGON_RESIZE_HANDLE_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

#include "include/models/polygon_model_item.h"

namespace optgui {

class PolygonResizeHandle : public QGraphicsEllipseItem {
 public:
    explicit PolygonResizeHandle(PolygonModelItem *model,
                                 quint32 index,
                                 QGraphicsItem *parent,
                                 qreal size = 14);
    // update graphic with model info
    void updatePos();
    // update model
    void updateModel(QPointF diff);
    // unique type of graphic
    int type() const override;
    // get model point
    QPointF getPoint();
    // draw graphic
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

 protected:
    // detect mouse input
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    // data model
    PolygonModelItem *model_;
    // index of point in model
    quint32 index_;

    // flag for mouse input
    bool resize_;

    // graphic size
    qreal size_;

    // scale zoom level
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // POLYGON_RESIZE_HANDLE_H_
