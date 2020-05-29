// TITLE:   Optimization_Interface/include/graphics/ellipse_resize_handle.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical resize handle for modifying ellipse constraints

#ifndef ELLIPSE_RESIZE_HANDLE_H_
#define ELLIPSE_RESIZE_HANDLE_H_

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

#include "include/models/ellipse_model_item.h"

namespace optgui {

class EllipseResizeHandle : public QGraphicsEllipseItem {
 public:
    explicit EllipseResizeHandle(EllipseModelItem *model,
                                 QGraphicsItem *parent,
                                 quint8 type,
                                 qreal size = 14);
    int type() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

 protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    void expandScene();
    EllipseModelItem *model_;
    bool resize_;
    qreal size_;
    quint8 type_;
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // ELLIPSE_RESIZE_HANDLE_H_
