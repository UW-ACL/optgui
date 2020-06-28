// TITLE:   Optimization_Interface/include/globals.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Global constants

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <QGraphicsItem>

namespace optgui {
    extern qreal const GRID_SIZE;  // scale from meters to pixels
    extern qreal const INIT_CLEARANCE;  // clearance around obs in meters

    // Color scheme constants
    extern QColor const RED;
    extern QColor const ORANGE;
    extern QColor const YELLOW;
    extern QColor const GREEN;
    extern QColor const CYAN;
    extern QColor const BLACK;

    // Toggle button state
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

    // Unique graphics object types
    enum GRAPHICS_TYPE {
        ELLIPSE_GRAPHIC = QGraphicsItem::UserType + 1,
        POLYGON_GRAPHIC = QGraphicsItem::UserType + 2,
        PLANE_GRAPHIC = QGraphicsItem::UserType + 3,
        WAYPOINT_GRAPHIC = QGraphicsItem::UserType + 4,
        ELLIPSE_HANDLE_GRAPHIC = QGraphicsItem::UserType + 5,
        PLANE_HANDLE_GRAPHIC = QGraphicsItem::UserType + 6,
        POLYGON_HANDLE_GRAPHIC = QGraphicsItem::UserType + 7,
        POINT_GRAPHIC = QGraphicsItem::UserType + 8
    };

    // Traj feasibility
    enum FEASIBILITY_CODE {
        FEASIBLE,
        INFEASIBLE
    };

    // Input validation
    enum INPUT_CODE {
        VALID_INPUT,
        OBS_OVERLAP,
        DRONE_OVERLAP,
        FINAL_POS_OVERLAP
    };

    // scale from meters to pixels and transform from ned to xyz
    QPointF nedToGuiXyz(qreal n, qreal e);

    // scale from pixels to meters and transform from xyz to ned
    QPointF guiXyzToNED(qreal x, qreal y);
    QPointF guiXyzToNED(QPointF const &gui_coords);

}  // namespace optgui

#endif  // GLOBALS_H_
