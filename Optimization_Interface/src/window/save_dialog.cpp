// TITLE:   Optimization_Interface/src/window/save_dialog.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/save_dialog.h"

#include <QVBoxLayout>
#include <QBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>

namespace optgui {

SaveDialog::SaveDialog(QWidget *parent)
    : QDialog(parent, Qt::Tool) {

    // Set title
    this->setWindowTitle("Save Config file");

    // Set default size
    this->setMinimumSize(300, 400);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create layout
    this->setLayout(new QVBoxLayout(this));
}

SaveDialog::~SaveDialog() {
    delete this->layout();
}

void SaveDialog::saveConfig(ConstraintModel *model) {

    // Popup to select filename
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            "../untitled.config" );
    QFile file(fileName);

    // Get models in current window
    ellipses_ = model->getEllipses();
    polygons_ = model->getPolygons();
    waypoints_ = model->getWaypoints();
    final_points_ = model->getPoints();
    drone_ = model->getDrones();

    // Write data to file
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        // Ellipses
        for (QVector<EllipseModelItem *>::iterator ptr = ellipses_.begin(); ptr != ellipses_.end(); ++ptr) {
            stream << "Ellipse:" << endl;
            stream << "\tWidth: " << (*ptr)->getWidth() << endl; // width in pixels
            stream << "\tHeight: " << (*ptr)->getHeight() << endl; // height in pixels
            stream << "\tRotation: " << (*ptr)->getRot() << endl; // clockwise rotation
            stream << "\tPosition: " << (*ptr)->getPos().x() << ", " << (*ptr)->getPos().y() << endl; // x position in pixels
            stream << "\tClearance: " << (*ptr)->getClearance() << endl; // clearance in meters
            stream << "\tPort: " << (*ptr)->port_ << endl; // port
        }
        // Waypoints
        for (QVector<PointModelItem *>::iterator ptr = waypoints_.begin(); ptr != waypoints_.end(); ++ptr) {
            stream << "\nWaypoints:" << endl;
            stream << "\tPosition: " << (*ptr)->getPos().x() << ", " << (*ptr)->getPos().y() << endl; // y position in pixels
            stream << "\tPort: " << (*ptr)->port_ << endl; // port
        }
        // Polygons
        for (QSet<PolygonModelItem *>::iterator ptr = polygons_.begin(); ptr != polygons_.end(); ++ptr) {
             stream << "\nPolygons:" << endl;
             quint32 n_vert = (*ptr)->getSize();
             for (quint32 i=0; i<n_vert; i++){
                stream << "\tVertex: " << (*ptr)->getPointAt(i).x() << ", " << (*ptr)->getPointAt(i).y() << endl; // number of vertices
             }
             stream << "\tPort: " << (*ptr)->port_ << endl; // port
        }
        // Final Points
        for (QSet<PointModelItem *>::iterator ptr = final_points_.begin(); ptr != final_points_.end(); ++ptr) {
             stream << "\nFinal Points:" << endl;
             stream << "\tPosition: " << (*ptr)->getPos().x() << ", " << (*ptr)->getPos().y() << endl; // x position in pixels
             stream << "\tPort: " << (*ptr)->port_ << endl; // port
        }
        // Drones
        if (sizeof(drone_) > 0) {
             stream << "\nDrone:" << endl;
             stream << "\tPosition: " << (drone_)->getPos().x() << ", " << (drone_)->getPos().y() << endl; // x position in pixels
             stream << "\tPort: " << (drone_)->port_ << endl; // port
        }
        stream << "\nEnd" << endl;
    }
    
}


}  // namespace optgui
