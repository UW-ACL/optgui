// TITLE:   Optimization_Interface/canvas.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsScene>

namespace interface {

const qreal GRID_SIZE = 100;

class Canvas : public QGraphicsScene
{
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

}  // namespace

#endif // CANVAS_H
