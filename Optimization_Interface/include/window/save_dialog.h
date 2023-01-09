// TITLE:   Optimization_Interface/include/window/save_dialog.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Pop up dialog for saving configuration file

#ifndef SAVE_DIALOG_H_
#define SAVE_DIALOG_H_

#include <QDialog>
#include <QTableWidget>

#include "include/models/constraint_model.h"

namespace optgui {

class SaveDialog : public QDialog {
    Q_OBJECT

 public:
    explicit SaveDialog(QWidget *parent = nullptr);
    ~SaveDialog();

    // save configuration file
    void saveConfig(ConstraintModel *model);

    QVector<EllipseModelItem *> ellipses_;
    QSet<PolygonModelItem *> polygons_;
    QVector<PointModelItem *> waypoints_;
    QSet<PointModelItem *> final_points_;
    DroneModelItem* drone_;

};

}  // namespace optgui

#endif  // SAVE_DIALOG_H_
