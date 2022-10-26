// TITLE:   Optimization_Interface/src/window/port_dialog.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/port_dialog.h"

#include <QVBoxLayout>
#include <QBoxLayout>
#include <QHeaderView>

#include "include/window/port_dialog/port_selector.h"

namespace optgui {

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

void PortDialog::fillTable(ConstraintModel *model) {
    model->fillTable(this->port_table_,
                            this->drone_table_,
                            this->ports_);
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
            new QTableWidgetItem(tr("Listening ID")));
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
//    this->drone_table_->setMinimumSize(
//                this->drone_table_->minimumSize().rwidth(), 30);
//    this->drone_table_->setMaximumSize(
//                this->drone_table_->maximumSize().rwidth(), 32);

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

}  // namespace optgui
