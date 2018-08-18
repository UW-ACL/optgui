#include "view.h"

View::View(QWidget *parent)
{
    //this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    //this->zoom();
}

void View::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    //this->zoom();
    //this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}

bool View::compareRect(QRectF rect1, QRectF rect2) {
    return rect1.width() > rect2.width() || rect1.height() > rect1.height();
}

void View::zoom() {
    QRectF itemBound = this->scene()->itemsBoundingRect();
    QRectF sceneBound = this->sceneRect();
    QRectF viewBound = this->rect();
    QRectF unified = itemBound.united(sceneBound);

    if (compareRect(unified, viewBound)) {
        this->setSceneRect(unified);
    } else {
        this->setSceneRect(viewBound);
    }
}

void View::paintEvent(QPaintEvent *event)
{
    //this->scene()->setSceneRect(this->rect());
    QGraphicsView::paintEvent(event);
    //this->zoom();
    //this->centerOn(this->scene()->itemsBoundingRect().center());
    //this->centerOn(this->sceneRect().center());
    //this->ensureVisible(this->sceneRect());
    //this->scale(this->size().width() / this->sceneRect().width(), this->size().height() / this->sceneRect().height());
}
