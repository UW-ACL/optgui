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
INCLUDEPATH += $$PWD/../../mikipilot/build/gcs/executable/release/

LIBS += -L$$PWD/../../skyenet/algorithm -lalgorithm # //SKYENET// looks for libalgorithm.a file
LIBS += -L$$PWD/../../skyenet/cprs/build -lCPRS     # looks for libCPRS.a
LIBS += -L$$PWD/../../skyenet/csocp -lCSOCP         # looks for libCSOCP.a

# //MIKIPILOT//
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_autogen_globals     # looks for lib_autogen_globals.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_autogen_packet      # looks for lib_autogen_packet.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_autogen_state       # looks for lib_autogen_state.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_autogen_parameter   # looks for lib_autogen_parameter.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_autogen_timestamped # looks for lib_autogen_timestamped.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_autogen_bus         # looks for lib_autogen_bus.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_network             # looks for lib_network.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_utilities           # looks for lib_utilities.a
LIBS += -L$$PWD/../../mikipilot/build/gcs/executable/release/ -l_gnc                 # looks for lib_gnc.a

SOURCES += \
    src/main.cpp \
    src/window/main_window.cpp \
    src/graphics/canvas.cpp \
    src/graphics/view.cpp \
    src/window/menu_panel.cpp \
    src/window/menu_button.cpp \
    src/controls/controller.cpp \
    src/models/constraint_model.cpp \
    src/graphics/ellipse_graphics_item.cpp \
    src/graphics/ellipse_resize_handle.cpp \
    src/graphics/polygon_graphics_item.cpp \
    src/graphics/polygon_resize_handle.cpp \
    src/graphics/plane_graphics_item.cpp \
    src/graphics/drone_graphics_item.cpp \
    src/graphics/waypoints_graphics_item.cpp \
    src/graphics/path_graphics_item.cpp \
    src/window/port_dialog.cpp \
    src/window/port_selector.cpp \
    src/network/item_server.cpp \
    src/network/drone_server.cpp \
    src/network/path_server.cpp \
    src/network/ellipse_server.cpp \
    src/network/polygon_server.cpp \
    src/network/plane_server.cpp \
    src/graphics/point_graphics_item.cpp \
    src/network/point_server.cpp \
#    ../../mikipilot/gcs/comm.cpp
    src/network/comm.cpp

HEADERS += \
    include/window/main_window.h \
    include/graphics/canvas.h \
    include/graphics/view.h \
    include/window/menu_panel.h \
    include/window/menu_button.h \
    include/globals.h \
    include/controls/controller.h \
    include/models/constraint_model.h \
    include/models/ellipse_model_item.h \
    include/graphics/ellipse_graphics_item.h \
    include/graphics/ellipse_resize_handle.h \
    include/graphics/polygon_graphics_item.h \
    include/models/polygon_model_item.h \
    include/graphics/polygon_resize_handle.h \
    include/models/plane_model_item.h \
    include/graphics/plane_graphics_item.h \
    include/graphics/drone_graphics_item.h \
    include/models/path_model_item.h \
    include/models/drone_model_item.h \
    include/graphics/waypoints_graphics_item.h \
    include/graphics/path_graphics_item.h \
    include/window/port_dialog.h \
    include/window/port_selector.h \
    include/models/data_model.h \
    include/network/item_server.h \
    include/network/drone_server.h \
    include/network/path_server.h \
    include/network/ellipse_server.h \
    include/network/polygon_server.h \
    include/network/plane_server.h \
    include/network/point_server.h \
    include/models/point_model_item.h \
    include/graphics/point_graphics_item.h \
#    ../../mikipilot/gcs/comm.h
    include/network/comm.h

RESOURCES += \
    resources.qrc
