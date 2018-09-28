#include "view.h"

#include <QDebug>

View::View(QGraphicsScene *scene, QWidget * parent)
    : QGraphicsView(scene, parent)
{
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    this->setRenderHint(QPainter::Antialiasing);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->openButton = new MenuButton(this, Qt::LeftArrow);
    this->controls = new ControlBox(this);

    this->setLayout(new QHBoxLayout);
    this->layout()->addWidget(this->controls);
    this->layout()->setMargin(0);
    this->layout()->setAlignment(this->controls, Qt::AlignRight);
    this->layout()->addWidget(this->openButton);
    this->layout()->setAlignment(this->openButton, Qt::AlignRight);

    this->controls->hide();
    this->setZoom();
    this->placeCircle = false;

    connect(this->openButton, SIGNAL(clicked()), this, SLOT(openMenu()));
    connect(this->controls->closeButton, SIGNAL(clicked()), this, SLOT(closeMenu()), Qt::DirectConnection);
    connect(this->controls->controlPanel->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setZoom()));

    connect(this->controls->controlPanel->circleButton, SIGNAL(circleClicked()), this, SLOT(circleClicked()));
    connect(this, SIGNAL(circlePlaced()), this->controls->controlPanel->circleButton, SLOT(circlePlaced()));
}

void View::openMenu() {
    this->openButton->hide();
    this->controls->show();
    repaint();
}

void View::closeMenu() {
    this->controls->hide();
    this->openButton->show();
    repaint();
}

void View::setZoom() {
    this->scaleFactor = qreal(this->controls->controlPanel->zoomSlider->value() - 1) / 5;
    this->resetMatrix();
    this->scale(scaleFactor, scaleFactor);

    repaint();
}

void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    QRectF oldView = this->viewport()->rect();
    oldView.translate(-oldView.center().x(), -oldView.center().y());

    this->scene()->setSceneRect((this->scene()->sceneRect()).united(oldView));
    this->setSceneRect(this->scene()->sceneRect());
}

void View::mousePressEvent(QMouseEvent *event)
{
    if (this->placeCircle) {
        qreal size = 25;
        Obstacle *circle = new Obstacle(nullptr, size);
        this->scene()->addItem(circle);
        qreal xPos = event->pos().x() - size;
        qreal yPos = event->pos().y() - size;
        circle->setPos(this->mapToScene(QPoint(xPos, yPos)));
        emit circlePlaced();
        this->placeCircle = false;
    }

    QGraphicsView::mousePressEvent(event);
}

void View::circleClicked() {
    this->placeCircle = true;
}



