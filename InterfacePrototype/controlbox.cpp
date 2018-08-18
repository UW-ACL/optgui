#include "controlbox.h"

#include <QVBoxLayout>
#include <QPushButton>

ControlBox::ControlBox(QWidget *parent, Qt::WindowFlags flags)
    : QFrame(parent, flags)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setFixedWidth(150);
    setAutoFillBackground(true);
    QPalette palette = this->palette();
    QColor background = QWidget::palette().window().color();
    background.setAlpha(200);
    palette.setColor(QPalette::Base, background);
    this->setPalette(palette);
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);

    this->closeButton = new MenuButton(this, Qt::RightArrow);

    this->setLayout(new QHBoxLayout);
    this->layout()->addWidget(this->closeButton);
    this->layout()->setMargin(1);
    this->layout()->setAlignment(this->closeButton, Qt::AlignLeft);

    this->controlPanel = new ControlPanel(this);
    this->layout()->addWidget(this->controlPanel);
    this->layout()->setAlignment(this->controlPanel, Qt::AlignTop );


}


void ControlBox::resizeEvent(QResizeEvent *event)
{
//    if (this->closeButton != nullptr && this->closeButton->isVisible()) {
//        QRectF parentRect = this->rect();
//        //qreal moveX = parentRect.right() - this->closeButton->width();
//        qreal moveY = (parentRect.height() - this->closeButton->height())/2;
//        this->closeButton->move(0, moveY);
//    }
    QFrame::resizeEvent(event);
}
