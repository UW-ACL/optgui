// TITLE:   Optimization_Interface/include/graphics/path_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for current drone path

#ifndef PATH_GRAPHICS_ITEM_H_
#define PATH_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "include/globals.h"
#include "include/models/path_model_item.h"

namespace optgui {

class PathGraphicsItem : public QGraphicsItem {
 public:
    explicit PathGraphicsItem(PathModelItem *model,
                              quint32 Ksub,
                              QGraphicsItem *parent = nullptr,
                              quint32 size = 4);
    // rough area of graphic
    QRectF boundingRect() const override;
    // draw graphic
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    // set color of traj
    void setColor(QColor);

    PathModelItem *model_;

 protected:
    // shape to draw
    QPainterPath shape() const override;
    // update model when graphic is changed
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;

 private:
    QPen pen_;
    QPen waypoints_pen_;
    quint32 width_;
    QMutex mutex_;  // mutex lock for compute thread setting color
    qreal getScalingFactor() const;
    quint32 Ksub_;
};

}  // namespace optgui

#endif  // PATH_GRAPHICS_ITEM_H_
