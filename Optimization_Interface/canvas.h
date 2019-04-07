// TITLE:   Optimization_Interface/canvas.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Component holding all graphical representations of data

#ifndef CANVAS_H_
#define CANVAS_H_

#include <QGraphicsScene>
#include <QFont>

namespace interface {

const qreal GRID_SIZE = 100;
const QString UNIT = QString("m");

class Canvas : public QGraphicsScene {
    Q_OBJECT

 public:
    explicit Canvas(QObject *parent = nullptr);
    void bringToFront(QGraphicsItem *item);
    void expandScene();
 protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
 private slots:
    void bringSelectedToFront();
 private:
    void initialize();
    QPen background_pen_;
    QPen foreground_pen_;
    qreal front_depth_;
    qint64 roundUpPast(qint64 n, qint64 m);
    qint64 roundDownPast(qint64 n, qint64 m);
    QFont font_;
};

}  // namespace interface

#endif  // CANVAS_H_
