// TITLE:   Optimization_Interface/globals.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QGraphicsItem>

namespace interface {
    enum STATE {
        IDLE,
        ERASER,
        FLIP,
        ELLIPSE,
        POLYGON,
        PLANE,
        PATH
    };

    enum GRAPHICS_TYPE {
        ELLIPSE_GRAPHIC = QGraphicsItem::UserType + 1,
        POLYGON_GRAPHIC = QGraphicsItem::UserType + 2,
        PLANE_GRAPHIC = QGraphicsItem::UserType + 3,
        PATH_GRAPHIC = QGraphicsItem::UserType + 4
    };
}  // namespace

#endif // GLOBALS_H
