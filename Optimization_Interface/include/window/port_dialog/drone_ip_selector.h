// TITLE:   Optimization_Interface/include/window/drone_ip_selector.h
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

// Entry box for setting drone destination ipv4 address

#ifndef DRONE_IP_SELECTOR_H_
#define DRONE_IP_SELECTOR_H_

#include <QLineEdit>

#include "include/models/drone_model_item.h"

namespace interface {

class DroneIpSelector : public QLineEdit {
    Q_OBJECT

 public:
    explicit DroneIpSelector(DroneModelItem *model, QWidget *parent);

 protected:
    void focusInEvent(QFocusEvent *event) override;

 public slots:
    void updateIp();

 private:
    DroneModelItem *model_;
    bool isIpValid();
};

}  // namespace interface

#endif  // DRONE_IP_SELECTOR_H_
