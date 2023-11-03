// TITLE:   Optimization_Interface/src/window/load_dialog.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/load_dialog.h"

#include <QVBoxLayout>
#include <QBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

namespace optgui {

LoadDialog::LoadDialog(QWidget *parent)
    : QDialog(parent, Qt::Tool) {

    // Set title
    this->setWindowTitle("Load Config file");

    // Set default size
    this->setMinimumSize(300, 400);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create layout
    this->setLayout(new QVBoxLayout(this));
}

LoadDialog::~LoadDialog() {
    delete this->layout();
}

void LoadDialog::loadConfig(ConstraintModel *model) {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"),
                            "../Optimization_Interface/" );

    QFile file(fileName);
    
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    
    // Variables for stepping through file
    QTextStream in(&file);
    QRegExp separator("[:,]");
    // Variables to keep track of current item
    char new_item_type;
    char old_item_type = NULL;
    bool new_item = 1;
    // Polygon vertices variable
    QVector<QPointF> vertices;

    // Step through config file
    while(!in.atEnd()) {
        // Read current line
        QStringList line = in.readLine().split(separator);

        if (line[0] == "")
            continue;

        // First check if new item
        if (line[0] == "Ellipse"){
            new_item_type = 'e';
            new_item = !new_item;
            continue;
        }
        else if (line[0] == "Waypoints"){
            new_item_type = 'w';
            new_item = !new_item;
            continue;
        }
        else if (line[0] == "Polygons"){
            new_item_type = 'p';
            new_item = !new_item;
            QVector<QPointF> vertices;
            continue;
        }
        else if (line[0] == "Final Points"){
            new_item_type = 'f';
            new_item = !new_item;
            continue;
        }
        else if (line[0] == "Drone"){
            new_item_type = 'd';
            new_item = !new_item;
            continue;
        }
        else if (line[0] == "End"){
            new_item = !new_item;
        }

        // If old_item_type is null, set it
       if (old_item_type == NULL){
           old_item_type = new_item_type;
       }

        // If new item, add last item to model
        if (new_item){
            if (old_item_type == 'e'){
                // create new data model
                EllipseModelItem *item_model = new EllipseModelItem(pos, cle, hei, wid, rot);
                if (por != NULL)
                    item_model->port_ = por;
                // create graphic based on data model and save to model
                model->addEllipse(item_model);
            }
            else if (old_item_type == 'w'){
                // create new data model
                PointModelItem *item_model = new PointModelItem(pos);
                if (por != NULL)
                    item_model->port_ = por;
                // create graphic based on data model and save to model
                model->addWaypoint(item_model);
            }
            else if (old_item_type == 'p'){
                // create new data model
               PolygonModelItem *item_model = new PolygonModelItem(vertices);
               if (por != NULL)
                    item_model->port_ = por;
               // create graphic based on data model and save to model
               model->addPolygon(item_model);
            }
            else if (old_item_type == 'f'){
                // create new data model
                PointModelItem *item_model = new PointModelItem(pos);
                if (por != NULL)
                    item_model->port_ = por;
                // create graphic based on data model and save to model
                model->addPoint(item_model);
            }
            else if (old_item_type == 'd'){
                // create new data model
                DroneModelItem *item_model = new DroneModelItem(pos);
                PathModelItem *traj;
                if (por != NULL)
                    item_model->port_ = por;
                // create graphic based on data model and save to model
                model->addDrone(item_model, traj);
            }
            new_item = 0;
            old_item_type = new_item_type;
        }

        // Store item parameters
        if (line[0].contains("Position")){
            pos = QPoint(line[1].toDouble(), line[2].toDouble());
        }
        else if (line[0].contains("Height")){
            hei = line[1].toDouble();
        }
        else if (line[0].contains("Rotation")){
            rot = line[1].toDouble();
        }
        else if (line[0].contains("Width")){
            wid = line[1].toDouble();
        }
        else if (line[0].contains("Clearance")){
            cle = line[1].toDouble();
        }
        else if (line[0].contains("Port")){
            por = line[1].toDouble();
        }
        else if(line[0].contains("Vertex")){
            pos = QPointF(line[1].toDouble(), line[2].toDouble());
            vertices.append(pos);
        }

    }

    file.close();
    
}



}  // namespace optgui
