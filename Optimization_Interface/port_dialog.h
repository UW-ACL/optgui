// TITLE:   Optimization_Interface/port_dialog.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Pop up dialog for setting port numbers

#ifndef PORT_DIALOG_H_
#define PORT_DIALOG_H_

#include <QDialog>
#include <QTableWidget>

#include "constraint_model.h"

namespace interface {

class PortDialog : public QDialog {
    Q_OBJECT

 public:
    explicit PortDialog(QWidget *parent = nullptr);
    ~PortDialog();
    void setModel(ConstraintModel *model);
 private:
    void initializeTable();
    QTableWidget *table_;
    ConstraintModel *model_;
    QSet<quint16> *ports_;
};

}  // namespace interface

#endif  // PORT_DIALOG_H_
