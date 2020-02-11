// TITLE:   Optimization_Interface/include/window/drone_port_selector.h
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

// Entry box for setting drone destination port

#ifndef DRONE_PORT_SELECTOR_H_
#define DRONE_PORT_SELECTOR_H_

#include <QLineEdit>

#include "include/models/drone_model_item.h"

namespace optgui {

class DronePortSelector : public QLineEdit {
    Q_OBJECT

 public:
    explicit DronePortSelector(DroneModelItem *model, QWidget *parent);

 protected:
    void focusInEvent(QFocusEvent *event) override;

 public slots:
    void updatePort();

 private:
    DroneModelItem *model_;
    bool isPortValid();
};

}  // namespace optgui

#endif  // DRONE_PORT_SELECTOR_H_
