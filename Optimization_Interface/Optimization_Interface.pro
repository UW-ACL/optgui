#-------------------------------------------------
#
# Project created by QtCreator 2018-12-08T16:54:57
#
#-------------------------------------------------

QT       += core gui
QT       += network

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

#MIKIPILOT = $$PWD/../../../mikipilot


INCLUDEPATH += $$PWD/../../skyenet/algorithm/
INCLUDEPATH += $$PWD/../../skyenet/cprs/headers/
INCLUDEPATH += $$PWD/../../skyenet/csocp/
INCLUDEPATH += $$PWD/../../mikipilot

LIBS += -L$$PWD/../../skyenet/algorithm -lalgorithm # //SKYENET// looks for libalgorithm.a file
LIBS += -L$$PWD/../../skyenet/cprs/build -lCPRS     # looks for libCPRS.a
LIBS += -L$$PWD/../../skyenet/csocp -lCSOCP         # looks for libCSOCP.a


## Add libraries.
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/utilities/globals.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/utilities/ifile.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/utilities/object.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/utilities/ofile.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/utilities/timer.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/utilities/tob.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/network/deserializable.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/network/packet.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/network/parameter.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/network/serializable.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/network/state.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/network/timestamped.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/gnc/dcm.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/gnc/globals.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/objects/gnc/quat.o
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/release/lib_autogen_globals.a
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/release/lib_autogen_packet.a
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/release/lib_autogen_state.a
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/release/lib_autogen_parameter.a
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/release/lib_autogen_timestamped.a
LIBS += $${PWD}/../../mikipilot/build/gcs/executable/release/lib_autogen_bus.a

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
    port_selector.cpp \
    item_server.cpp \
    drone_server.cpp \
    path_server.cpp \
    ellipse_server.cpp \
    polygon_server.cpp \
    plane_server.cpp \
    point_graphics_item.cpp \
    point_server.cpp \
#    ../../mikipilot/gcs/comm.cpp
    comm.cpp

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
    data_model.h \
    item_server.h \
    drone_server.h \
    path_server.h \
    ellipse_server.h \
    polygon_server.h \
    plane_server.h \
    point_server.h \
    point_model_item.h \
    point_graphics_item.h \
#    ../../mikipilot/gcs/comm.h
    comm.h

RESOURCES += \
    resources.qrc
