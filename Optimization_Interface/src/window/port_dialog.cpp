// TITLE:   Optimization_Interface/src/window/port_dialog.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/port_dialog.h"

#include <QVBoxLayout>
#include <QBoxLayout>
#include <QHeaderView>

#include "include/window/port_selector.h"
#include "include/window/drone_port_selector.h"
#include "include/window/drone_ip_selector.h"

namespace interface {

PortDialog::PortDialog(QWidget *parent)
    : QDialog(parent, Qt::Tool) {
    // Set title
    this->setWindowTitle("Set Ports");

    // Set default size
    this->setMinimumSize(300, 400);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create layout
    this->setLayout(new QVBoxLayout(this));

    // Set table
    this->initializeTable();
    this->ports_ = new QSet<quint16>();

    // connect slots
    this->connect(this, SIGNAL(finished(int)), this, SLOT(resetTable()));
}

PortDialog::~PortDialog() {
    delete this->ports_;

    this->layout()->removeWidget(this->port_table_);
    delete this->port_table_;

    this->layout()->removeWidget(this->drone_table_);
    delete this->drone_table_;

    delete this->layout();
}

void PortDialog::setModel(ConstraintModel *model) {
    this->model_ = model;
    this->resetTable();

    // Configure port table
    this->port_table_->setRowCount(3 + this->model_->ellipses_->size() +
                              this->model_->polygons_->size() +
                              this->model_->planes_->size());

    // Set drone
    this->port_table_->setItem(0, 0, new QTableWidgetItem(tr("Drone")));
    this->port_table_->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    this->ports_->insert(this->model_->drone_->port_);
    this->port_table_->setCellWidget(0, 1,
            new PortSelector(this->ports_, this->model_->drone_,
                             this->port_table_));

    // Set final point
    this->port_table_->setItem(1, 0, new QTableWidgetItem(tr("Final Point")));
    this->port_table_->item(1, 0)->setFlags(Qt::ItemIsEnabled);
    this->ports_->insert(this->model_->final_pos_->port_);
    this->port_table_->setCellWidget(1, 1,
            new PortSelector(this->ports_, this->model_->final_pos_,
                             this->port_table_));

    // Set waypoints
    this->port_table_->setItem(2, 0, new QTableWidgetItem(tr("Waypoints")));
    this->port_table_->item(2, 0)->setFlags(Qt::ItemIsEnabled);
    this->ports_->insert(this->model_->waypoints_->port_);
    this->port_table_->setCellWidget(2, 1,
            new PortSelector(this->ports_, this->model_->waypoints_,
                             this->port_table_));

    quint16 row = 3;
    // Set ellipses
    quint16 count = 1;
    for (EllipseModelItem *model : *this->model_->ellipses_) {
        this->port_table_->setItem(row, 0,
                new QTableWidgetItem(tr("Ellipse ") + QString::number(count)));
        this->port_table_->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        this->port_table_->setCellWidget(row, 1,
                new PortSelector(this->ports_, model, this->port_table_));
        this->ports_->insert(model->port_);

        row++;
        count++;
    }

    // Set polygons
    count = 1;
    for (PolygonModelItem *model : *this->model_->polygons_) {
        this->port_table_->setItem(row, 0,
                new QTableWidgetItem(tr("Polygon ") + QString::number(count)));
        this->port_table_->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        this->port_table_->setCellWidget(row, 1,
                new PortSelector(this->ports_, model, this->port_table_));
        this->ports_->insert(model->port_);

        row++;
        count++;
    }

    // Set planes
    count = 1;
    for (PlaneModelItem *model : *this->model_->planes_) {
        this->port_table_->setItem(row, 0,
                new QTableWidgetItem(tr("Plane ") + QString::number(count)));
        this->port_table_->item(row, 0)->setFlags(Qt::ItemIsEnabled);

        this->port_table_->setCellWidget(row, 1,
                new PortSelector(this->ports_, model, this->port_table_));
        this->ports_->insert(model->port_);

        row++;
        count++;
    }

    // Set drone table
    this->drone_table_->setRowCount(2);

    // Set drone ip
    this->drone_table_->setItem(0, 0,
                                new QTableWidgetItem(tr("Drone Ip Addr")));
    this->drone_table_->item(0, 0)->setFlags(Qt::ItemIsEnabled);
    this->drone_table_->setCellWidget(0, 1,
            new DroneIpSelector(this->model_->drone_, this->drone_table_));

    // Set drone destination port
    this->drone_table_->setItem(1, 0, new QTableWidgetItem(tr("Drone Port")));
    this->drone_table_->item(1, 0)->setFlags(Qt::ItemIsEnabled);
    this->drone_table_->setCellWidget(1, 1,
            new DronePortSelector(this->model_->drone_, this->drone_table_));
}

void PortDialog::resetTable() {
    // table takes ownership of pointers and deletes them
    this->port_table_->clearContents();
    this->drone_table_->clearContents();
    this->ports_->clear();
}

void PortDialog::initializeTable() {
    // Create tables
    this->port_table_ = new QTableWidget(this);
    this->drone_table_ = new QTableWidget(this);

    // Set headers for port table
    this->port_table_->setColumnCount(2);
    this->port_table_->setHorizontalHeaderItem(0,
            new QTableWidgetItem(tr("Graphics Item")));
    this->port_table_->setHorizontalHeaderItem(1,
            new QTableWidgetItem(tr("Listening Port")));
    this->port_table_->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);
    this->port_table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->port_table_->horizontalHeader()->
            setSectionResizeMode(QHeaderView::Stretch);
    this->port_table_->verticalHeader()->setVisible(false);

    // Set drone table
    this->drone_table_->setColumnCount(2);
    this->drone_table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->drone_table_->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    this->drone_table_->horizontalHeader()->
            setSectionResizeMode(QHeaderView::Stretch);
    this->drone_table_->verticalHeader()->setVisible(false);
    this->drone_table_->horizontalHeader()->setVisible(false);
    this->drone_table_->setMinimumSize(
                this->drone_table_->minimumSize().rwidth(), 61);
    this->drone_table_->setMaximumSize(
                this->drone_table_->maximumSize().rwidth(), 62);

    // Set color
    this->port_table_->setAutoFillBackground(true);
    this->drone_table_->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QColor background = QWidget::palette().window().color();
    palette.setColor(QPalette::Base, background);
    this->setPalette(palette);

    // Add to layout
    this->layout()->addWidget(this->drone_table_);
    this->layout()->addWidget(this->port_table_);
}

void PortDialog::closeEvent(QCloseEvent *event) {
    // update all ports
    for (int i = 0; i < this->port_table_->rowCount(); i++) {
        PortSelector *selector = qobject_cast<PortSelector*>
                (this->port_table_->cellWidget(i, 1));
        selector->updatePort();
    }

    emit setSocketPorts();  // create sockets for objects
    QDialog::closeEvent(event);
}

}  // namespace interface
