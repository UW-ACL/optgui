// TITLE:   Optimization_Interface/ellipse_resize_handle.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef ELLIPSE_RESIZE_HANDLE_H
#define ELLIPSE_RESIZE_HANDLE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

#include "ellipse_model_item.h"

namespace interface {

const qreal ELLIPSE_HANDLE_SIZE = 16;

class EllipseResizeHandle : public QGraphicsEllipseItem
{
public:
    EllipseResizeHandle(EllipseModelItem *model, QGraphicsItem *parent);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
private:
    void expandScene();
    EllipseModelItem *model_;
    bool resize_;
};

}  // namespace

#endif // ELLIPSE_RESIZE_HANDLE_H
