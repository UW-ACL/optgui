#include "controlpanel.h"

#include <QPushButton>

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setLayout(new QVBoxLayout(this));

    this->circleButton = new CircleButton(this);
    this->zoomSlider = new ZoomSlider(this);
    this->eraserButton = new EraserButton(this);
    this->polyButton = new PolyButton(this);
    this->lineButton = new LineButton(this);
    this->startButton = new StartButton(this);
    this->vectorButton = new VectorButton(this);

    this->layout()->addWidget(this->circleButton);
    this->layout()->addWidget(this->polyButton);
    this->layout()->addWidget(this->lineButton);
    this->layout()->addWidget(this->startButton);
    this->layout()->addWidget(this->vectorButton);
    this->layout()->addItem(new QSpacerItem(50, 50, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->layout()->addWidget(this->eraserButton);
    this->layout()->addWidget(this->zoomSlider);

    this->layout()->setAlignment(this->circleButton, Qt::AlignTop|Qt::AlignLeft);
    this->layout()->setAlignment(this->polyButton, Qt::AlignTop|Qt::AlignLeft);
    this->layout()->setAlignment(this->lineButton, Qt::AlignTop|Qt::AlignLeft);
    this->layout()->setAlignment(this->startButton, Qt::AlignTop|Qt::AlignLeft);
    this->layout()->setAlignment(this->vectorButton, Qt::AlignTop|Qt::AlignLeft);

    this->layout()->setAlignment(this->eraserButton, Qt::AlignBottom|Qt::AlignLeft);
    this->layout()->setAlignment(this->zoomSlider, Qt::AlignBottom);
}
