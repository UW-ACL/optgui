// TITLE:   Optimization_Interface/include/globals.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Global constants

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <QGraphicsItem>

namespace optgui {
    static qreal const GRID_SIZE = 100.0;
    static qreal const INIT_CLEARANCE = 0.5;

    static QColor const RED = QColor(0xF6, 0x40, 0x3D);
    static QColor const ORANGE = QColor(0xFD, 0x85, 0x30);
    static QColor const YELLOW = QColor(0xE8, 0xC6, 0x38);
    static QColor const GREEN = QColor(0x37, 0xD2, 0x24);
    static QColor const CYAN = QColor(0x55, 0xE2, 0xD2);
    static QColor const BLACK = QColor(0x1B, 0x1B, 0x1B);

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

    enum FEEDBACK_CODE {
        FEASIBLE,
        OBS_OVERLAP,
        GENERIC_INFEASIBLE
    };

    static QPointF nedToGuiXyz(qreal n, qreal e) {
        // QPointF(x, y ,z)
        return QPointF(e * GRID_SIZE, -1.0 * n * GRID_SIZE);
    }

    static QPointF guiXyzToNED(qreal x, qreal y) {
        // QPointF(n, e, d)
        return QPointF(-1.0 * y / GRID_SIZE, x / GRID_SIZE);
    }

    static QPointF guiXyzToNED(QPointF const &gui_coords) {
        // QPointF(n, e, d)
        return QPointF(-1.0 * gui_coords.y() / GRID_SIZE,
                       gui_coords.x() / GRID_SIZE);
    }
}  // namespace optgui

#endif  // GLOBALS_H_
