// TITLE:   Optimization_Interface/src/window/drone_id_selector.cpp
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

#include "include/window/port_dialog/drone_id_selector.h"

#include <QTimer>
#include <QSet>

namespace optgui {

DroneIdSelector::DroneIdSelector(DroneModelItem *model, QWidget *parent,
                                 QSet<quint16> *ports)
    : QLineEdit(parent) {
    this->ports_ = ports;
    this->model_ = model;

    // display current IP address
    this->setText(this->model_->ip_addr_.split(".").last());

    this->connect(this, SIGNAL(editingFinished()), this, SLOT(updateIp()));
}

void DroneIdSelector::focusInEvent(QFocusEvent *event) {
    // select all text when clicked
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void DroneIdSelector::updateIp() {
    // remove old port from used ports list
    this->ports_->remove(this->model_->port_);

    // check that ip addr is valid ipv4 form
    quint16 value = this->isIpValid();
    if (value != 0) {
        // generate IP address and port from drone ID
        this->model_->ip_addr_ = "192.168.1." + this->text();
        if (value < 8000){
            this->model_->port_ = 8000 + value;
            // add new port to list
            this->ports_->insert(8000 + value);
        } else{
            this->model_->port_ = value;
            // add new port to list
            this->ports_->insert(value);
        }
    } else {
        this->setText("0");
        this->model_->ip_addr_ = "0.0.0.0";
        this->model_->port_ = 0;
    }
}

quint16 DroneIdSelector::isIpValid() {
    // validate id
    bool ok = false;
    quint16 value = this->text().toUShort(&ok);
    if (ok && 0 < value && value < 256 &&
            !this->ports_->contains(8000 + value)) {
        return value;
    }
    return 0;
}

}  // namespace optgui
