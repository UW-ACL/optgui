#include "menubutton.h"

MenuButton::MenuButton(QWidget *parent, Qt::ArrowType arrow)
    : QToolButton(parent)
{
    this->setArrowType(arrow);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setFixedSize(10, 40);
    //this->setBaseSize(20, 40);
}
