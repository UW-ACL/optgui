#include "zoomslider.h"

ZoomSlider::ZoomSlider(QWidget *parent) : QSlider(Qt::Horizontal, parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    this->setTickInterval(1);
    this->setTickPosition(QSlider::TicksAbove);
    this->setMinimum(3);
    this->setMaximum(9);
    this->setValue(6);
}
