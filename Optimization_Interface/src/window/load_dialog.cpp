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

QSet<EllipseModelItem *> LoadDialog::loadConfig(ConstraintModel *model) {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"),
                            "../Optimization_Interface/" );

    QFile file(fileName);
    
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }

    QSet<EllipseModelItem *> ellipses_;
    QSet<PolygonModelItem *> polygons_;
    QVector<PointModelItem *> waypoints_;
    QSet<PointModelItem *> final_points_;
    
    QTextStream in(&file);
    QRegExp separator("[:,]");
    qreal wid;
    qreal hei;
    qreal rot;
    QPointF pos;
    qreal cle;

    while(!in.atEnd()) {
        QString line = in.readLine();    
        QStringList fields = line.split(":");
        if (fields[0] == "Ellipse"){
            QString itemtype = "e";
            QStringList width = in.readLine().split(separator);
            if (width[0].contains("Width")){
                wid = width[1].toDouble();
            }
            QStringList height = in.readLine().split(separator);
            if (height[0].contains("Height")){
                hei = height[1].toDouble();
            }
            QStringList rotation = in.readLine().split(separator);
            if (rotation[0].contains("Rotation")){
                rot = rotation[1].toDouble();
            }
            QStringList position = in.readLine().split(separator);
            if (position[0].contains("Position")){
                pos = QPoint(position[1].toDouble(), position[2].toDouble());
            }
            QStringList clear = in.readLine().split(separator);
            if (clear[0].contains("Clearance")){
                cle = clear[1].toDouble();
            }

            // create new data model
            EllipseModelItem *item_model = new EllipseModelItem(pos, cle, hei, wid, rot);
            // create graphic based on data model and save to model
            ellipses_.insert(item_model);
        }
        if (fields[0] == "Waypoints"){
            QString itemtype = "w";
            QStringList pos = in.readLine().split(separator);
        }
        if (fields[0] == "Polygons"){
            QString itemtype = "p";
            // for (){
            //     QStringList pos = in.readLine().split(separator); // TODO
            // }
        }
        if (fields[0] == "Final Points"){
            QString itemtype = "f";
            QStringList pos = in.readLine().split(separator);
        }
    }

    file.close();

    return ellipses_;
    
}



}  // namespace optgui
