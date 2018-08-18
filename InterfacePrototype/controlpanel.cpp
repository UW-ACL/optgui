#include "controlpanel.h"

#include <QPushButton>

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent)
{
    this->setLayout(new QVBoxLayout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    this->circleButton = new CircleButton(this);
    this->layout()->addWidget(this->circleButton);
    //this->layout()->addWidget(new QPushButton("Test", this));

}
