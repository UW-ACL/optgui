#include "view.h"

#include <QDebug>

View::View(QGraphicsScene *scene, QWidget * parent)
    : QGraphicsView(scene, parent)
{
    this->setAcceptDrops(true);
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

    connect(this->openButton, SIGNAL(clicked()), this, SLOT(openMenu()));
    connect(this->controls->closeButton, SIGNAL(clicked()), this, SLOT(closeMenu()), Qt::DirectConnection);
    connect(this->controls->controlPanel->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setZoom()));
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


void View::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("circle-obstacle")) {
        event->acceptProposedAction();
    }
}

void View::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("circle-obstacle")) {
        event->acceptProposedAction();
    }
}

void View::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("circle-obstacle")) {
        const QMimeData *mime = event->mimeData();
        QByteArray itemData = mime->data("circle-obstacle");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPoint offset;
        qreal size;
        dataStream >> offset >> size;

        event->acceptProposedAction();
        this->scene()->clearSelection();
        Obstacle *circle = new Obstacle(nullptr, size);
        this->scene()->addItem(circle);
        circle->setPos(mapToScene(event->pos()) - offset);
    }
}



