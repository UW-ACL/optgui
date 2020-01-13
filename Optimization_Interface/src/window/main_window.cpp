// TITLE:   Optimization_Interface/src/window/main_window.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/window/main_window.h"

#include <QMenuBar>
#include <QObject>

namespace interface {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::Window) {
    // Set title
    this->setWindowTitle("Optimization Interface");

    // Set default size
    this->setMinimumSize(1024, 576);
    // this->showFullScreen();

    // Set view
    this->view_ = new View(this);

    // Add view to layout
    this->setCentralWidget(this->view_);

    // Initialize menu items
    this->initializeMenu();

    // Only need if malloc main window
    // Close properly on quit
    // this->setAttribute(Qt::WA_DeleteOnClose);
}

MainWindow::~MainWindow() {
    // Delete menu items
//    delete this->load_file_;
//    delete this->save_file_;
    delete this->set_ports_;

    // delete menu
    delete this->file_menu_;

    // Delete view
    // TODO(dtsullivan): automatically deleted my parent?
    delete this->view_;
}

void MainWindow::initializeMenu() {
    // Create file menu
    this->file_menu_ = this->menuBar()->addMenu(tr("&File"));

    // TODO(dtsull16): re-enable save/load functionality after refactoring is
    // done and https://github.com/dtsullivan/optgui/issues/7#issue-504936751
    // is completed
//    // Initialize load file action
//    this->load_file_ = new QAction(tr("&Open"), this->file_menu_);
//    this->load_file_->setShortcuts(QKeySequence::Open);
//    this->load_file_->setToolTip(tr("Load layout from file"));
//    connect(this->load_file_, SIGNAL(triggered()),
//            this->view_, SLOT(loadFile()));

//    // Initialize save file action
//    this->save_file_ = new QAction(tr("&Save"), this->file_menu_);
//    this->save_file_->setShortcuts(QKeySequence::Save);
//    this->save_file_->setToolTip(tr("Save current layout to file"));
//    connect(this->save_file_, SIGNAL(triggered()),
//            this->view_, SLOT(saveFile()));

    // Initialize set ports file action
    this->set_ports_ = new QAction(tr("&Set Ports"), this->file_menu_);
    this->set_ports_->setToolTip(tr("Set ports for constraints"));
    connect(this->set_ports_, SIGNAL(triggered()),
            this->view_, SLOT(setPorts()));

    // Add actions to menu
//    this->file_menu_->addAction(this->load_file_);
//    this->file_menu_->addAction(this->save_file_);
    this->file_menu_->addAction(this->set_ports_);
}

}  // namespace interface
