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

#include "include/graphics/view.h"

namespace optgui {

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

 private:
    // set up window menu
    void initializeMenu();
    // main window widget
    View *view_;
    // window menu
    QMenu *file_menu_;
    // save/load functionality (not currently supported)
//    QAction *save_file_;
//    QAction *load_file_;
    // open network configuration dialog
    QAction *set_ports_;
};

}  // namespace optgui

#endif  // MAIN_WINDOW_H_
