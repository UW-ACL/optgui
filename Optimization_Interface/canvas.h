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
    explicit Canvas(QObject *parent = nullptr, QString background_file = "");
    void bringToFront(QGraphicsItem *item);
    void expandScene();
    QPointF* getBottomLeft();
    QPointF* getTopRight();
    bool indoor_ = true;

 protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;
 private slots:
    void bringSelectedToFront();
 private:
    void initialize();
    void setBackgroundImage(QString filename);
    QPen background_pen_;
    QPen foreground_pen_;
    qreal front_depth_;
    qint64 roundUpPast(qint64 n, qint64 m);
    qint64 roundDownPast(qint64 n, qint64 m);
    QFont font_;
    QImage *background_image_;

    double background_bottomleft_x_ = 0;
    double background_bottomleft_y_ = 0;
    double background_topright_x_ = 1;
    double background_topright_y_ = 1;
    // TODO: fix this...
    double scale_ = 100;
};

}  // namespace interface

#endif  // CANVAS_H_
