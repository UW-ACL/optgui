// TITLE:   Optimization_Interface/include/graphics/cylinder_resize_handle.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical resize handle for modifying cylinder constraints

#ifndef CYLINDER_RESIZE_HANDLE_H_
#define CYLINDER_RESIZE_HANDLE_H_

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

#include "include/models/cylinder_model_item.h"

namespace optgui {

class CylinderResizeHandle : public QGraphicsRectItem {
 public:
    explicit CylinderResizeHandle(CylinderModelItem *model,
                                 QGraphicsItem *parent,
                                 quint8 type,
                                 qreal size = 14);
    // unique type for graphic
    int type() const override;
    // draw graphic
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

 protected:
    // respond to mouse input
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

 private:
    // data model
    CylinderModelItem *model_;

    // flag for moving handle
    bool resize_;

    // size of handle
    qreal size_;

    // handle manipulates: width == 0, height == 1, both >= 2
    quint8 type_;

    // scale zoom level
    qreal getScalingFactor() const;
};

}  // namespace optgui

#endif  // CYLIDNER_RESIZE_HANDLE_H_
