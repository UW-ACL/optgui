// TITLE:   Optimization_Interface/main_window.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "main_window.h"

namespace interface {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::Window)
{
    // Set title
    this->setWindowTitle("Optimiztion Interface");

    // Set default size
    this->setMinimumSize(600, 400);
    // this->resize(800, 600);
    this->showFullScreen();

    // Set view
    this->view_ = new View(this);

    // Add view to layout
    this->setCentralWidget(this->view_);
}

MainWindow::~MainWindow()
{
    delete this->view_;
}

}  // namespace
