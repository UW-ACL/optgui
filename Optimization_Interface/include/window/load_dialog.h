// TITLE:   Optimization_Interface/include/window/save_dialog.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Pop up dialog for loading configuration file

#ifndef LOAD_DIALOG_H_
#define LOAD_DIALOG_H_

#include <QDialog>
#include <QTableWidget>

#include "include/models/constraint_model.h"
#include "include/models/ellipse_model_item.h"

namespace optgui {

class LoadDialog : public QDialog {
    Q_OBJECT

 public:
    explicit LoadDialog(QWidget *parent = nullptr);
    ~LoadDialog();

    // load configuration file
    QSet<EllipseModelItem *> loadConfig(ConstraintModel *model);

};

}  // namespace optgui

#endif  // LOAD_DIALOG_H_
