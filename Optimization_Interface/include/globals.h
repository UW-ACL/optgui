// TITLE:   Optimization_Interface/include/globals.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Global constants

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <QGraphicsItem>

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
        POINT
    };

    enum GRAPHICS_TYPE {
        ELLIPSE_GRAPHIC = QGraphicsItem::UserType + 1,
        POLYGON_GRAPHIC = QGraphicsItem::UserType + 2,
        PLANE_GRAPHIC = QGraphicsItem::UserType + 3,
        WAYPOINTS_GRAPHIC = QGraphicsItem::UserType + 4,
        ELLIPSE_HANDLE_GRAPHIC = QGraphicsItem::UserType + 5,
        PLANE_HANDLE_GRAPHIC = QGraphicsItem::UserType + 6,
        POLYGON_HANDLE_GRAPHIC = QGraphicsItem::UserType + 7,
        POINT_GRAPHIC = QGraphicsItem::UserType + 8
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

    QPointF nedToGuiXyz(qreal n, qreal e);
    QPointF guiXyzToNED(qreal x, qreal y);
    QPointF guiXyzToNED(QPointF const &gui_coords);
}  // namespace optgui

#endif  // GLOBALS_H_
