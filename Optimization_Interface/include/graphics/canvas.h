// TITLE:   Optimization_Interface/include/graphics/canvas.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Component holding all graphical representations of data

#ifndef CANVAS_H_
#define CANVAS_H_

#include <QGraphicsScene>
#include <QFont>

#include "include/graphics/path_graphics_item.h"
#include "include/graphics/drone_graphics_item.h"
#include "include/graphics/waypoints_graphics_item.h"
#include "include/graphics/point_graphics_item.h"
#include "include/graphics/ellipse_graphics_item.h"
#include "include/graphics/polygon_graphics_item.h"
#include "include/graphics/plane_graphics_item.h"

namespace optgui {

class Canvas : public QGraphicsScene {
    Q_OBJECT

 public:
    explicit Canvas(QObject *parent = nullptr, QString background_file = "");
    ~Canvas();
    void bringToFront(QGraphicsItem *item);
    void expandScene();
    QPointF* getBottomLeft();
    QPointF* getTopRight();
    bool indoor_ = true;

    WaypointsGraphicsItem *waypoints_graphic_;
    PathGraphicsItem *path_graphic_;
    PathGraphicsItem *path_sent_graphic_;
    DroneGraphicsItem *drone_graphic_;
    PointGraphicsItem *final_point_;

    QSet<EllipseGraphicsItem *> *ellipse_graphics_;
    QSet<PolygonGraphicsItem *> *polygon_graphics_;
    QSet<PlaneGraphicsItem *> *plane_graphics_;

 protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

 public slots:
    void bringSelectedToFront();
    void updateEllipseGraphicsItem(EllipseGraphicsItem *graphic);
    void updatePathGraphicsItem();

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
    // TODO(bchasnov): fix this...
    double scale_ = 100;
};

}  // namespace optgui

#endif  // CANVAS_H_
