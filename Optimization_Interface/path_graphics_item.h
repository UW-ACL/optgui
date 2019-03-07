// TITLE:   Optimization_Interface/path_graphics_item.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Graphical representation of waypoint

#ifndef PATH_GRAPHICS_ITEM_H_
#define PATH_GRAPHICS_ITEM_H_

#include <QGraphicsItem>
#include <QPainter>
#include <QPointF>
#include <QVector>

namespace interface {

const qreal PATH_POINT_SIZE = 20;

class PathGraphicsItem : public QGraphicsItem {
 public:
    explicit PathGraphicsItem(QPointF *point, QVector<QPointF *> *path,
                              QGraphicsItem *parent = nullptr);
    QPointF *point_;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    int type() const override;

    void expandScene();
 protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;
 private:
    void initialize();
    QPen pen_;
    QBrush brush_;
    QVector<QPointF *> *path_;
};

}  // namespace interface

#endif  // PATH_GRAPHICS_ITEM_H_
