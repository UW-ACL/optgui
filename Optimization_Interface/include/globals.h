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
        HANDLE_GRAPHIC = QGraphicsItem::UserType + 5,
        POINT_GRAPHIC = QGraphicsItem::UserType + 6
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
