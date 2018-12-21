// TITLE:   Optimization_Interface/main.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include <QApplication>

#include "main_window.h"

using interface::MainWindow;

int main(int argc, char *argv[])
{
    // Initialize application
    QApplication app(argc, argv);

    // Create main window
    MainWindow window;
    window.show();

    // Run event loop
    return app.exec();
}
