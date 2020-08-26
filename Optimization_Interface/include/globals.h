// TITLE:   Optimization_Interface/include/globals.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Global constants

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <QGraphicsItem>
#include <QVector3D>

namespace optgui {
    extern qreal const GRID_SIZE;
    extern qreal const INIT_CLEARANCE;

    extern QColor const RED;
    extern QColor const ORANGE;
    extern QColor const YELLOW;
    extern QColor const GREEN;
    extern QColor const CYAN;
    extern QColor const BLACK;

    enum STATE {
        IDLE,
        ERASER,
        FLIP,
        ELLIPSE,
        POLYGON,
        PLANE,
        WAYPOINT,
        POINT,
        DRONE
    };

    enum GRAPHICS_TYPE {
        ELLIPSE_GRAPHIC = QGraphicsItem::UserType + 1,
        POLYGON_GRAPHIC = QGraphicsItem::UserType + 2,
        PLANE_GRAPHIC = QGraphicsItem::UserType + 3,
        WAYPOINT_GRAPHIC = QGraphicsItem::UserType + 4,
        ELLIPSE_HANDLE_GRAPHIC = QGraphicsItem::UserType + 5,
        PLANE_HANDLE_GRAPHIC = QGraphicsItem::UserType + 6,
        POLYGON_HANDLE_GRAPHIC = QGraphicsItem::UserType + 7,
        POINT_GRAPHIC = QGraphicsItem::UserType + 8,
        DRONE_GRAPHIC = QGraphicsItem::UserType + 9
    };

    enum FEASIBILITY_CODE {
        FEASIBLE,
        INFEASIBLE
    };

    enum INPUT_CODE {
        VALID_INPUT,
        OBS_OVERLAP,
        DRONE_OVERLAP,
        FINAL_POS_OVERLAP
    };

    QVector3D nedToGuiXyz(qreal n, qreal e, qreal d);
//    QPointF guiXyzToNED(qreal x, qreal y);
//    QPointF guiXyzToNED(QPointF const &gui_coords);
    QVector3D guiXyzToXyz(QVector3D const &gui_coords);
    QVector3D guiXyzToXyz(qreal x, qreal y, qreal z);
    QVector3D xyzToGuiXyz(QVector3D const &xyz_coords);
    QVector3D xyzToGuiXyz(qreal x, qreal y, qreal z);
}  // namespace optgui

#endif  // GLOBALS_H_
