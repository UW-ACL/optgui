// TITLE:   Optimization_Interface/path_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation for current drone path

#ifndef PATH_GRAPHICS_ITEM_H_
#define PATH_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>

#include "path_model_item.h"

namespace interface {

class PathGraphicsItem : public QGraphicsItem {
 public:
    explicit PathGraphicsItem(PathModelItem *model,
                                QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    void expandScene();

    void setColor(QColor);
 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;
 private:
    void initialize();
    PathModelItem *model_;
    QPen pen_;
};

}  // namespace interface

#endif  // PATH_GRAPHICS_ITEM_H_
