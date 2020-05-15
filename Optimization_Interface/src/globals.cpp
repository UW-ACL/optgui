// TITLE:   Optimization_Interface/include/globals.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/globals.h"

namespace optgui {
    qreal const GRID_SIZE = 100.0;
    qreal const INIT_CLEARANCE = 0.5;

    QColor const RED = QColor(0xF6, 0x40, 0x3D);
    QColor const ORANGE = QColor(0xFD, 0x85, 0x30);
    QColor const YELLOW = QColor(0xE8, 0xC6, 0x38);
    QColor const GREEN = QColor(0x37, 0xD2, 0x24);
    QColor const CYAN = QColor(0x55, 0xE2, 0xD2);
    QColor const BLACK = QColor(0x1B, 0x1B, 0x1B);

    QPointF nedToGuiXyz(qreal n, qreal e) {
        // QPointF(x, y ,z)
        return QPointF(e * GRID_SIZE, -1.0 * n * GRID_SIZE);
    }

    QPointF guiXyzToNED(qreal x, qreal y) {
        // QPointF(n, e, d)
        return QPointF(-1.0 * y / GRID_SIZE, x / GRID_SIZE);
    }

    QPointF guiXyzToNED(QPointF const &gui_coords) {
        // QPointF(n, e, d)
        return QPointF(-1.0 * gui_coords.y() / GRID_SIZE,
                       gui_coords.x() / GRID_SIZE);
    }
}  // namespace optgui
