// TITLE:   Optimization_Interface/src/window/drone_ip_selector.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/window/port_dialog/drone_ip_selector.h"

#include <QTimer>

namespace interface {

DroneIpSelector::DroneIpSelector(DroneModelItem *model, QWidget *parent)
    : QLineEdit(parent) {
    this->model_ = model;

    this->setText(this->model_->ip_addr_);

    this->connect(this, SIGNAL(editingFinished()), this, SLOT(updateIp()));
}

void DroneIpSelector::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void DroneIpSelector::updateIp() {
    // check that ip addr is valid ipv4 form
    if (this->isIpValid()) {
        this->model_->ip_addr_ = this->text();
    } else {
        this->setText("0.0.0.0");
        this->model_->ip_addr_ = "0.0.0.0";
    }
}

bool DroneIpSelector::isIpValid() {
    // validate ip address is long enough
    QStringList ip_addr_sections_ = this->text().split(".");
    if (ip_addr_sections_.size() != 4) {
        return false;
    }

    // validate all sections of ipv4 address are valid
    for (QString addr_section_ : ip_addr_sections_) {
        bool ok = false;
        quint16 value = addr_section_.toUShort(&ok);
        if (!ok || value > 256) {
            return false;
        }
    }
    return true;
}

}  // namespace interface
