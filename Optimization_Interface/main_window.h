// TITLE:   Optimization_Interface/main_window.h
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>

#include "view.h"

namespace interface {

class MainWindow : public QMainWindow
{
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
};

}  // namespace

#endif // MAIN_WINDOW_H
