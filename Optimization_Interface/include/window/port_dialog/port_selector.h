// TITLE:   Optimization_Interface/include/window/port_selector.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Entry box for setting port numbers

#ifndef PORT_SELECTOR_H_
#define PORT_SELECTOR_H_

#include <QLineEdit>
#include <QSet>

#include "include/models/data_model.h"

namespace optgui {

class PortSelector : public QLineEdit {
    Q_OBJECT

 public:
    explicit PortSelector(QSet<quint16> *ports,
                          DataModel *model, QWidget *parent);

 protected:
    void focusInEvent(QFocusEvent *event) override;

 public slots:
    void updatePort();

 private:
    QSet<quint16> *ports_;
    DataModel *model_;
    bool isPortValid();
};

}  // namespace optgui

#endif  // PORT_SELECTOR_H_
