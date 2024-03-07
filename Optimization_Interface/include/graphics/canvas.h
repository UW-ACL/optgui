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
#include "include/graphics/waypoint_graphics_item.h"
#include "include/graphics/point_graphics_item.h"
#include "include/graphics/ellipse_graphics_item.h"
#include "include/graphics/cylinder_graphics_item.h"
#include "include/graphics/polygon_graphics_item.h"
#include "include/graphics/plane_graphics_item.h"

namespace optgui {

class Canvas : public QGraphicsScene {
    Q_OBJECT

 public:
    explicit Canvas(QObject *parent = nullptr, QString background_file = "");
    ~Canvas();

    // render item at top level
    void bringToFront(QGraphicsItem *item);

    QSet<PathGraphicsItem *> path_graphics_;
    PathGraphicsItem *path_staged_graphic_;

    QSet<DroneGraphicsItem *> drone_graphics_;
    QVector<EllipseGraphicsItem *> ellipse_graphics_;
    QVector<CylinderGraphicsItem *> cylinder_graphics_;
    QSet<PolygonGraphicsItem *> polygon_graphics_;
    QSet<PlaneGraphicsItem *> plane_graphics_;
    QVector<PointGraphicsItem *> final_points_;
    QVector<WaypointGraphicsItem *> waypoint_graphics_;

 protected:
    // draw foreground and background
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

 public slots:
    // move selected item to top render level
    void bringSelectedToFront();

    // manually force a re-render of item
    void updateGraphicsItems(PathGraphicsItem *, PathGraphicsItem *, DroneGraphicsItem *);

 private:
    qreal getScalingFactor();
    void setBackgroundImage(QString filename);
    QImage background_image_;

    // member variables for graphical style
    QPen background_pen_;
    QPen foreground_pen_;
    QFont font_;

    // max render level, used for move selected to front
    qreal front_depth_;

    // helper functions for drawing grid lines
    qint64 roundUpPast(qint64 n, qint64 m);
    qint64 roundDownPast(qint64 n, qint64 m);

    // size of backgorund image
    qreal background_bottomleft_x_ = 0;
    qreal background_bottomleft_y_ = 0;
    qreal background_topright_x_ = 1;
    qreal background_topright_y_ = 1;
};

}  // namespace optgui

#endif  // CANVAS_H_
