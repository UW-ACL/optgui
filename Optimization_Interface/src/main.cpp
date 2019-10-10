// TITLE:   Optimization_Interface/src/main.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Runs main event loop

#include <QApplication>

#include "../include/window/main_window.h"

using interface::MainWindow;

int main(int argc, char *argv[]) {
    qRegisterMetaType<packet::traj3dof>("packet::traj3dof");
    qRegisterMetaType<packet::telemetry>("packet::telemetry");

    // Initialize application
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    // Create main window
    MainWindow window;
    window.show();

    // Run event loop
    return app.exec();
}
