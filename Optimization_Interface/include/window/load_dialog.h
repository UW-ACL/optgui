// TITLE:   Optimization_Interface/include/window/save_dialog.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Load class for creating and loading config file

#ifndef LOAD_DIALOG_H_
#define LOAD_DIALOG_H_

#include <QDialog>
#include <QTableWidget>

#include "include/models/constraint_model.h"
#include "include/models/ellipse_model_item.h"

namespace optgui {

class LoadDialog : public QDialog {
    Q_OBJECT

 public:
    explicit LoadDialog(QWidget *parent = nullptr);
    ~LoadDialog();

    // load configuration file
    void loadConfig(ConstraintModel *model);

    QVector<EllipseModelItem *> ellipses_;
    QVector<CylinderModelItem *> cylinders_;
    QSet<PolygonModelItem *> polygons_;
    QVector<PointModelItem *> waypoints_;
    QSet<PointModelItem *> final_points_;
    DroneModelItem* drone_;

    qreal wid;
    qreal hei;
    qreal rot;
    qreal trigwid;
    QPointF pos;
    qreal cle;
    quint16 por = NULL;

};

}  // namespace optgui

#endif  // LOAD_DIALOG_H_
