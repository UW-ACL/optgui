#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <circlebutton.h>

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr);
    CircleButton *circleButton;
};

#endif // CONTROLPANEL_H
