#ifndef CONTROLBOX_H
#define CONTROLBOX_H

#include <QFrame>
#include <QGraphicsView>
#include "menubutton.h"
#include "controlpanel.h"

class ControlBox : public QFrame
{
    Q_OBJECT

public:
    ControlBox(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::Widget);
    void resizeEvent(QResizeEvent *event);
    MenuButton *closeButton;
    ControlPanel *controlPanel;
};

#endif // CONTROLBOX_H
