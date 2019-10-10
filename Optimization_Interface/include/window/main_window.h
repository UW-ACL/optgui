// TITLE:   Optimization_Interface/include/window/main_window.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Main GUI window

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>
#include <QMenu>
#include <QAction>

#include "../graphics/view.h"

namespace interface {

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
 private:
    void initializeMenu();
    View *view_;
    QMenu *file_menu_;
    QAction *save_file_;
    QAction *load_file_;
    QAction *set_ports_;
    QAction *connect_;
    QAction *disconnect_;
};

}  // namespace interface

#endif  // MAIN_WINDOW_H_
