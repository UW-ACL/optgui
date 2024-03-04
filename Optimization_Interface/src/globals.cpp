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
    QColor const TRANS_YELLOW = QColor(0x66, 0x57, 0x18); // not technically transparent, just darker yellow, looks better in GUI
    QColor const TRANS_RED = QColor(0x75, 0x1e, 0x1c);
    // QColor const TRANS_YELLOW = QColor(0xE8, 0xC6, 0x38, 0x50);

    QVector3D nedToGuiXyz(qreal n, qreal e, qreal d) {
        // QPointF(x, y ,z)
        return QVector3D(e * GRID_SIZE,
                  -1.0 * n * GRID_SIZE,
                  -1.0 * d * GRID_SIZE);
    }

//    QPointF guiXyzToNED(qreal x, qreal y) {
//        // QPointF(n, e, d)
//        return QPointF(-1.0 * y / GRID_SIZE, x / GRID_SIZE);
//    }

    QVector3D guiXyzToNED(QVector3D const &gui_coords) {
        // QVector3D(n, e, d)
        return QVector3D(-1.0 * gui_coords.y() / GRID_SIZE,
                        gui_coords.x() / GRID_SIZE,
                        -1.0 * gui_coords.z() / GRID_SIZE);
    }

    QVector3D guiXyzToXyz(QVector3D const &gui_coords) {
        // QVector3D(x, y, z)
        return QVector3D(gui_coords.x() / GRID_SIZE,
                       -1.0 * gui_coords.y() / GRID_SIZE,
                       gui_coords.z() / GRID_SIZE);
    }

    QVector3D guiXyzToXyz(qreal x, qreal y, qreal z) {
        // QVector3D(x, y, z)
        return QVector3D(x / GRID_SIZE,
                         -1.0 * y / GRID_SIZE,
                         z / GRID_SIZE);
    }

    QVector3D xyzToGuiXyz(QVector3D const &xyz_coords) {
        // QVector3D(x, -y, z)
        return QVector3D(xyz_coords.x() * GRID_SIZE,
                       -1.0 * xyz_coords.y() * GRID_SIZE,
                       xyz_coords.z() * GRID_SIZE);
    }

    QVector3D xyzToGuiXyz(qreal x, qreal y, qreal z) {
        // QVector3D(x, -y, z)
        return QVector3D(x * GRID_SIZE,
                         -1.0 * y * GRID_SIZE,
                         z * GRID_SIZE);
    }
}  // namespace optgui
