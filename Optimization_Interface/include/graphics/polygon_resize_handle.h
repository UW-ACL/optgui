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
    void updatePos();
    void updateModel(QPointF diff);
    int type() const override;
    QPointF getPoint();
    void setColor(const QColor color);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    void expandScene();
    PolygonModelItem *model_;
    quint32 index_;
    bool resize_;
    qreal size_;
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // POLYGON_RESIZE_HANDLE_H_
