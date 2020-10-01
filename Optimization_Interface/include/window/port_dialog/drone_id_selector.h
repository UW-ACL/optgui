// TITLE:   Optimization_Interface/include/window/drone_id_selector.h
// AUTHORS: Daniel Sullivan
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2020, All Rights Reserved

// Entry box for setting drone destination ipv4 address

#ifndef DRONE_ID_SELECTOR_H_
#define DRONE_ID_SELECTOR_H_

#include <QLineEdit>

#include "include/models/drone_model_item.h"

namespace optgui {

class DroneIdSelector : public QLineEdit {
    Q_OBJECT

 public:
    explicit DroneIdSelector(DroneModelItem *model, QWidget *parent,
                             QSet<quint16> *ports);

 protected:
    // select current input when typing
    void focusInEvent(QFocusEvent *event) override;

 public slots:
    // valida IP address and save to data model
    void updateIp();

 private:
    // currently used ports, shared between all input boxes
    QSet<quint16> *ports_;
    // data model
    DroneModelItem *model_;
    // helper function to validate IP address
    quint16 isIpValid();
};

}  // namespace optgui

#endif  // DRONE_ID_SELECTOR_H_
