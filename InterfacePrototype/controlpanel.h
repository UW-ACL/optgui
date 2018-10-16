#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include "circlebutton.h"
#include "zoomslider.h"
#include "eraserbutton.h"
#include "polybutton.h"
#include "linebutton.h"

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr);
    CircleButton *circleButton;
    ZoomSlider *zoomSlider;
    EraserButton *eraserButton;
    PolyButton *polyButton;
    LineButton *lineButton;
};

#endif // CONTROLPANEL_H
