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

    this->setText(QString::number(this->model_->port_));

    this->connect(this, SIGNAL(editingFinished()), this, SLOT(updatePort()));
}

void PortSelector::focusInEvent(QFocusEvent *event) {
    QLineEdit::focusInEvent(event);
    QTimer::singleShot(0, this, SLOT(selectAll()));
}

void PortSelector::updatePort() {
    this->ports_->remove(this->model_->port_);

    // check that port is in valid range and unused
    if (this->isPortValid()) {
        quint32 value = this->text().toUShort();
        this->ports_->insert((quint16)value);
        this->model_->port_ = (quint16)value;
    } else {
        this->setText("0");
        this->model_->port_ = 0;
    }
}

bool PortSelector::isPortValid() {
    bool ok = false;
    quint32 value = this->text().toUShort(&ok);

    if (!ok || 1024 > value || value > 65535) {
        return false;
    }
    return true;
}

}  // namespace optgui
