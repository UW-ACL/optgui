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
    void setModel(ConstraintModel *model);
    void closeEvent(QCloseEvent *event) override;

 private slots:
    void resetTable();

 signals:
    void setSocketPorts();

 private:
    void initializeTable();
    QTableWidget *port_table_;
    QTableWidget *drone_table_;
    ConstraintModel *model_;
    QSet<quint16> *ports_;
};

}  // namespace optgui

#endif  // PORT_DIALOG_H_
