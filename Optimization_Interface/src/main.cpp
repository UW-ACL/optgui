// TITLE:   Optimization_Interface/src/main.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

// Runs main event loop

#include <QApplication>

#include "include/window/main_window.h"

using interface::MainWindow;

int main(int argc, char *argv[]) {
    qRegisterMetaType<autogen::packet::traj3dof>("autogen::packet::traj3dof");
    qRegisterMetaType<autogen::packet::telemetry>("autogen::packet::telemetry");

    // Initialize application
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);

    // Create main window
    MainWindow window;
    window.show();

    // Run event loop
    return app.exec();
}
