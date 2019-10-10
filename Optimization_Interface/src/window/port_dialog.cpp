// TITLE:   Optimization_Interface/src/window/port_dialog.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "../../include/window/port_dialog.h"

#include <QHBoxLayout>
#include <QHeaderView>

#include "../../include/window/port_selector.h"

namespace interface {

PortDialog::PortDialog(QWidget *parent)
    : QDialog(parent, Qt::Tool) {
    // Set title
    this->setWindowTitle("Set Ports");

    // Set default size
    this->setMinimumSize(300, 400);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create layout
    this->setLayout(new QHBoxLayout(this));

    // Set table
    this->initializeTable();
    this->ports_ = new QSet<quint16>();

    // connect slots
    this->connect(this, SIGNAL(finished(int)), this, SLOT(resetTable()));
}

PortDialog::~PortDialog() {
    delete this->ports_;

    this->layout()->removeWidget(this->table_);
    delete this->table_;

    delete this->layout();
}

void PortDialog::setModel(ConstraintModel *model) {
    this->model_ = model;
    this->resetTable();
    this->table_->setRowCount(3 + this->model_->ellipses_->size() +
                              this->model_->polygons_->size() +
                              this->model_->planes_->size());

    // Set drone
    this->table_->setItem(0, 0, new QTableWidgetItem(tr("Drone")));
    this->table_->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    this->ports_->insert(this->model_->drone_->port_);
    this->table_->setCellWidget(0, 1,
            new PortSelector(this->ports_, this->model_->drone_, this->table_));

    // Set waypoints
    this->table_->setItem(1, 0, new QTableWidgetItem(tr("Waypoints")));
    this->table_->item(1, 0)->setFlags(Qt::ItemIsEnabled);
    this->ports_->insert(this->model_->waypoints_->port_);
    this->table_->setCellWidget(1, 1,
            new PortSelector(this->ports_, this->model_->waypoints_,
                             this->table_));

    // Set path
    this->table_->setItem(2, 0, new QTableWidgetItem(tr("Drone Path")));
    this->table_->item(2, 0)->setFlags(Qt::ItemIsEnabled);
    this->ports_->insert(this->model_->path_->port_);
    this->table_->setCellWidget(2, 1,
            new PortSelector(this->ports_, this->model_->path_, this->table_));

    quint16 row = 3;
    // Set ellipses
    quint16 count = 1;
    for (EllipseModelItem *model : *this->model_->ellipses_) {
        this->table_->setItem(row, 0,
                new QTableWidgetItem(tr("Ellipsey ") + QString::number(count)));
        this->table_->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        this->table_->setCellWidget(row, 1,
                new PortSelector(this->ports_, model, this->table_));
        this->ports_->insert(model->port_);

        row++;
        count++;
    }

    // Set polygons
    count = 1;
    for (PolygonModelItem *model : *this->model_->polygons_) {
        this->table_->setItem(row, 0,
                new QTableWidgetItem(tr("Polygon ") + QString::number(count)));
        this->table_->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        this->table_->setCellWidget(row, 1,
                new PortSelector(this->ports_, model, this->table_));
        this->ports_->insert(model->port_);

        row++;
        count++;
    }

    // Set planes
    count = 1;
    for (PlaneModelItem *model : *this->model_->planes_) {
        this->table_->setItem(row, 0,
                new QTableWidgetItem(tr("Plane ") + QString::number(count)));
        this->table_->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        this->table_->setCellWidget(row, 1,
                new PortSelector(this->ports_, model, this->table_));
        this->ports_->insert(model->port_);

        row++;
        count++;
    }
}

void PortDialog::resetTable() {
    this->table_->clearContents();
    this->ports_->clear();
}

void PortDialog::initializeTable() {
    // Create table
    this->table_ = new QTableWidget(this);

    // Set headers
    this->table_->setColumnCount(2);
    this->table_->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Item")));
    this->table_->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Port")));
    this->table_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->table_->horizontalHeader()->
            setSectionResizeMode(QHeaderView::Stretch);
    this->table_->verticalHeader()->setVisible(false);

    // Set color
    this->table_->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QColor background = QWidget::palette().window().color();
    palette.setColor(QPalette::Base, background);
    this->setPalette(palette);

    // Add to layout
    this->layout()->addWidget(this->table_);
}

}  // namespace interface
