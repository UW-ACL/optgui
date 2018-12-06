#-------------------------------------------------
#
# Project created by QtCreator 2018-08-12T18:57:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InterfacePrototype
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
        dialog.cpp \
    view.cpp \
    obstacle.cpp \
    canvas.cpp \
    menubutton.cpp \
    controlbox.cpp \
    controlpanel.cpp \
    circlebutton.cpp \
    zoomslider.cpp \
    resizehandle.cpp \
    eraserbutton.cpp \
    polyobstacle.cpp \
    polydot.cpp \
    polybutton.cpp \
    polyhandle.cpp \
    linebutton.cpp \
    lineobstacle.cpp \
    startdot.cpp \
    startvector.cpp \
    startbutton.cpp \
    vectorbutton.cpp

HEADERS += \
        dialog.h \
    view.h \
    obstacle.h \
    canvas.h \
    menubutton.h \
    controlbox.h \
    controlpanel.h \
    circlebutton.h \
    zoomslider.h \
    resizehandle.h \
    eraserbutton.h \
    polyobstacle.h \
    polydot.h \
    polybutton.h \
    polyhandle.h \
    linebutton.h \
    lineobstacle.h \
    startdot.h \
    startvector.h \
    startbutton.h \
    vectorbutton.h

FORMS += \
        dialog.ui

RESOURCES += \
    resources.qrc
