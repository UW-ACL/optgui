// TITLE:   Optimization_Interface/src/window/drone_port_selector.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/window/drone_port_selector.h"

#include <QTimer>

namespace interface {

DronePortSelector::DronePortSelector(DroneModelItem *model, QWidget *parent)
    : QLineEdit(parent) {
    this->model_ = model;

    this->setText(QString::number(this->model_->destination_port_));

    this->connect(this, SIGNAL(editingFinished()), this, SLOT(updatePort()));
}

void DronePortSelector::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void DronePortSelector::updatePort() {
    // check that port is in valid range
    if (this->isPortValid()) {
        this->model_->destination_port_ = (quint16)this->text().toUShort();
    } else {
        this->setText("0");
        this->model_->destination_port_ = 0;
    }
}

bool DronePortSelector::isPortValid() {
    bool ok = false;
    quint64 value = this->text().toUShort(&ok);

    if (!ok || 1024 > value || value > 65535) {
        return false;
    }
    return true;
}

}  // namespace interface
