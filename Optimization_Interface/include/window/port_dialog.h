// TITLE:   Optimization_Interface/include/window/port_dialog.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Pop up dialog for setting port numbers

#ifndef PORT_DIALOG_H_
#define PORT_DIALOG_H_

#include <QDialog>
#include <QTableWidget>

#include "include/models/constraint_model.h"

namespace optgui {

class PortDialog : public QDialog {
    Q_OBJECT

 public:
    explicit PortDialog(QWidget *parent = nullptr);
    ~PortDialog();

    // fill table from the model
    void fillTable(ConstraintModel *model);
    // save network configuration to data models on close
    void closeEvent(QCloseEvent *event) override;

 private slots:
    // clear contents of table
    void resetTable();

 signals:
    // signal to save ports to data models
    void setSocketPorts();

 private:
    // set up table layout
    void initializeTable();
    // table for constraint listening ports
    QTableWidget *port_table_;
    // table for drone listening port and destination address
    QTableWidget *drone_table_;
    // currently used ports, shared between all input boxes
    QSet<quint16> *ports_;
};

}  // namespace optgui

#endif  // PORT_DIALOG_H_
