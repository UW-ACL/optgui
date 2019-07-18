// TITLE:   Optimization_Interface/main_window.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "main_window.h"

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
}

MainWindow::~MainWindow() {
    // Delete menu items
    delete this->load_file_;
    delete this->save_file_;
    delete this->set_ports_;
    delete this->connect_;
    delete this->disconnect_;

    // delete menu
    delete this->file_menu_;

    // Delete view
    delete this->view_;
}

void MainWindow::initializeMenu() {
    // Create file menu
    this->file_menu_ = this->menuBar()->addMenu(tr("&File"));

    // Initialize load file action
    this->load_file_ = new QAction(tr("&Open"), this->file_menu_);
    this->load_file_->setShortcuts(QKeySequence::Open);
    this->load_file_->setToolTip(tr("Load layout from file"));
    connect(this->load_file_, SIGNAL(triggered()),
            this->view_, SLOT(loadFile()));

    // Initialize save file action
    this->save_file_ = new QAction(tr("&Save"), this->file_menu_);
    this->save_file_->setShortcuts(QKeySequence::Save);
    this->save_file_->setToolTip(tr("Save current layout to file"));
    connect(this->save_file_, SIGNAL(triggered()),
            this->view_, SLOT(saveFile()));

    // Initialize set ports file action
    this->set_ports_ = new QAction(tr("&Set Ports"), this->file_menu_);
    this->set_ports_->setToolTip(tr("Set ports for constraints"));
    connect(this->set_ports_, SIGNAL(triggered()),
            this->view_, SLOT(setPorts()));

    // Initialize connect file action
    this->connect_ = new QAction(tr("&Connect"), this->file_menu_);
    this->connect_->setToolTip(tr("Spin up servers for items with ports"));
    connect(this->connect_, SIGNAL(triggered()),
            this->view_, SLOT(startServers()));

    // Initialize disconnect file action
    this->disconnect_ = new QAction(tr("&Disconnect"), this->file_menu_);
    this->disconnect_->setToolTip(tr("Close running servers"));
    connect(this->disconnect_, SIGNAL(triggered()),
            this->view_, SLOT(closeServers()));

    // Add actions to menu
    this->file_menu_->addAction(this->load_file_);
    this->file_menu_->addAction(this->save_file_);
    this->file_menu_->addAction(this->set_ports_);
    this->file_menu_->addAction(this->connect_);
    this->file_menu_->addAction(this->disconnect_);
}

}  // namespace interface
