// TITLE:   Optimization_Interface/canvas.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Component holding all graphical representations of data

#ifndef CANVAS_H_
#define CANVAS_H_

#include <QGraphicsScene>

namespace interface {

const qreal GRID_SIZE = 100;

class Canvas : public QGraphicsScene {
    Q_OBJECT

 public:
    explicit Canvas(QObject *parent = nullptr);
    void bringToFront(QGraphicsItem *item);
 protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
 private slots:
    void bringSelectedToFront();
 private:
    void initialize();
    QPen background_pen_;
    qreal front_depth_;
};

}  // namespace interface

#endif  // CANVAS_H_
