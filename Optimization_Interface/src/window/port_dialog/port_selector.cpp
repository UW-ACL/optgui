// TITLE:   Optimization_Interface/src/window/port_selector.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/port_dialog/port_selector.h"

#include <QTimer>

namespace optgui {

PortSelector::PortSelector(QSet<quint16> *ports,
                           DataModel *model, QWidget *parent)
    : QLineEdit(parent) {
    this->ports_ = ports;
    this->model_ = model;

    // dispaly current port
    this->setText(QString::number(this->model_->port_));

    this->connect(this, SIGNAL(editingFinished()), this, SLOT(updatePort()));
}

void PortSelector::focusInEvent(QFocusEvent *event) {
    // select all text when clicked
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void PortSelector::updatePort() {
    // remove old port from list
    this->ports_->remove(this->model_->port_);

    // check that port is in valid range and unused
    quint16 value = this->isPortValid();
    if (value != 0) {
        // add new port to list and save to data model
        this->ports_->insert(value);
        this->model_->port_ = value;
    } else {
        this->setText("0");
        this->model_->port_ = 0;
    }
}

quint16 PortSelector::isPortValid() {
    bool ok = false;
    quint16 value = this->text().toUShort(&ok);

    // validate port is in valid range and not already used
    if (ok && 1023 < value && value <= 65535 &&
            !this->ports_->contains(value)) {
        return value;
    }
    return 0;
}

}  // namespace optgui
