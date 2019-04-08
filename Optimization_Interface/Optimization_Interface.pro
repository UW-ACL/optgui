#-------------------------------------------------
#
# Project created by QtCreator 2018-12-08T16:54:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Optimization_Interface
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        main_window.cpp \
    canvas.cpp \
    view.cpp \
    menu_panel.cpp \
    menu_button.cpp \
    controller.cpp \
    constraint_model.cpp \
    ellipse_graphics_item.cpp \
    ellipse_resize_handle.cpp \
    polygon_graphics_item.cpp \
    polygon_resize_handle.cpp \
    plane_graphics_item.cpp \
    drone_graphics_item.cpp \
    waypoints_graphics_item.cpp \
    path_graphics_item.cpp \
    port_dialog.cpp \
    port_selector.cpp

HEADERS += \
        main_window.h \
    canvas.h \
    view.h \
    menu_panel.h \
    menu_button.h \
    globals.h \
    controller.h \
    constraint_model.h \
    ellipse_model_item.h \
    ellipse_graphics_item.h \
    ellipse_resize_handle.h \
    polygon_graphics_item.h \
    polygon_model_item.h \
    polygon_resize_handle.h \
    plane_model_item.h \
    plane_graphics_item.h \
    drone_graphics_item.h \
    path_model_item.h \
    drone_model_item.h \
    waypoints_graphics_item.h \
    path_graphics_item.h \
    port_dialog.h \
    port_selector.h \
    data_model.h

RESOURCES += \
    resources.qrc
