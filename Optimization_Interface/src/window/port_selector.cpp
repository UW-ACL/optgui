// TITLE:   Optimization_Interface/src/window/port_selector.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/port_selector.h"

#include <QTimer>

namespace interface {

PortSelector::PortSelector(QSet<quint16> *ports,
                           DataModel *model, QWidget *parent)
    : QLineEdit(parent) {
    this->ports_ = ports;
    this->model_ = model;

    this->setText(QString::number(this->model_->port_));

    this->connect(this, SIGNAL(editingFinished()), this, SLOT(updatePort()));
}

void PortSelector::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void PortSelector::updatePort() {
    qint64 value = this->text().toInt();
    this->ports_->remove(this->model_->port_);

    // check that port is in valid range and unused
    if (1024 <= value && value <= 65535 &&
            !this->ports_->contains((quint16)value)) {
        this->ports_->insert((quint16)value);
        this->model_->port_ = (quint16)value;
    } else {
        this->setText("0");
        this->model_->port_ = 0;
    }
}

}  // namespace interface
