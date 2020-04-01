// TITLE:   Optimization_Interface/src/window/drone_id_selector.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/window/port_dialog/drone_id_selector.h"

#include <QTimer>

namespace optgui {

DroneIdSelector::DroneIdSelector(DroneModelItem *model, QWidget *parent)
    : QLineEdit(parent) {
    this->model_ = model;

    this->setText(this->model_->ip_addr_.split(".").last());

    this->connect(this, SIGNAL(editingFinished()), this, SLOT(updateIp()));
}

void DroneIdSelector::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void DroneIdSelector::updateIp() {
    // check that ip addr is valid ipv4 form
    if (this->isIpValid()) {
        this->model_->ip_addr_ = "192.168.1." + this->text();
        this->model_->port_ = 8000 + this->text().toUShort();
    } else {
        this->setText("0");
        this->model_->ip_addr_ = "0.0.0.0";
        this->model_->port_ = 0;
    }
}

bool DroneIdSelector::isIpValid() {
    // validate id
    bool ok = false;
    quint16 value = this->text().toUShort(&ok);
    if (!ok || value > 256 || value < 1) {
        return false;
    }
    return true;
}

}  // namespace optgui
