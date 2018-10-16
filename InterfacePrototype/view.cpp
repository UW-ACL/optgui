#include "view.h"

View::View(QGraphicsScene *scene, QWidget * parent)
    : QGraphicsView(scene, parent)
{
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    this->setRenderHint(QPainter::Antialiasing);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

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
    this->eraserMode = false;

    connect(this->openButton, SIGNAL(clicked()), this, SLOT(openMenu())); // open menu
    connect(this->controls->closeButton, SIGNAL(clicked()), this, SLOT(closeMenu())); // close menu
    connect(this->controls->controlPanel->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setZoom())); // set zoom

    connect(this->controls->controlPanel->circleButton, SIGNAL(circleOn()), this, SLOT(circleOn()), Qt::DirectConnection); // circle mode on
    connect(this->controls->controlPanel->circleButton, SIGNAL(circleOff()), this, SLOT(circleOff()), Qt::DirectConnection); // circle mode off
    connect(this, SIGNAL(circleButtonOff()), this->controls->controlPanel->circleButton, SLOT(buttonOff()), Qt::DirectConnection); // circle placed

    connect(this->controls->controlPanel->eraserButton, SIGNAL(eraserOn()), this, SLOT(eraserOn()), Qt::DirectConnection); // eraser mode on
    connect(this->controls->controlPanel->eraserButton, SIGNAL(eraserOff()), this, SLOT(eraserOff()), Qt::DirectConnection); // eraser mode off
    connect(this, SIGNAL(eraserButtonOff()), this->controls->controlPanel->eraserButton, SLOT(buttonOff()), Qt::DirectConnection); // eraser button off

    connect(this->controls->controlPanel->polyButton, SIGNAL(polygonOn()), this, SLOT(polygonOn()), Qt::DirectConnection); // poly mode on
    connect(this->controls->controlPanel->polyButton, SIGNAL(polygonOff()), this, SLOT(polygonOff()), Qt::DirectConnection); // poly mode off
    connect(this, SIGNAL(polygonButtonOff()), this->controls->controlPanel->polyButton, SLOT(buttonOff()), Qt::DirectConnection); // poly button off

    connect(this->controls->controlPanel->lineButton, SIGNAL(lineOn()), this, SLOT(lineOn()), Qt::DirectConnection); // line mode on
    connect(this->controls->controlPanel->lineButton, SIGNAL(lineOff()), this, SLOT(lineOff()), Qt::DirectConnection); // line mode off
    connect(this, SIGNAL(lineButtonOff()), this->controls->controlPanel->lineButton, SLOT(buttonOff()), Qt::DirectConnection); // line button off


    connect(this->scene(), SIGNAL(selectionChanged()), this, SLOT(eraseItem())); // eraser item

    this->polyVector = new QVector<PolyDot*>();
    this->placePolygon = false;
    this->placeLine = false;
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

void View::eraseItem() {
    if (this->eraserMode) {
        foreach(QGraphicsItem *item, this->scene()->selectedItems()) {
            this->scene()->removeItem(item);
            item->~QGraphicsItem();
        }
    }
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
        Obstacle *circle = new Obstacle(size, nullptr);
        this->scene()->addItem(circle);
        circle->setPos(this->mapToScene(event->pos()));
        emit circleButtonOff();
    } else if (this->placePolygon) {
        if (this->polyVector->isEmpty()) {
            QPointF *point = new QPointF(this->mapToScene(event->pos()));
            PolyDot *dot = new PolyDot(point);
            polyVector->append(dot);
            this->scene()->addItem(dot);
            this->bringToFront(dot);
        } else {
            PolyDot *item = dynamic_cast<PolyDot*>(itemAt(event->pos()));
            if (item != nullptr && item == this->polyVector->first()) {
                polyVector->append(polyVector->first());
                QPolygonF poly;
                foreach(PolyDot *dot, *this->polyVector) {
                    poly << *dot->point;
                }
                if (poly.isClosed() && polyVector->size() > 3) {
                    QVector<QPointF*> *pointVector = new QVector<QPointF*>();
                    foreach(PolyDot *dot, *this->polyVector) {
                        pointVector->append(dot->point);
                    }
                    PolyObstacle *polyOb = new PolyObstacle(pointVector);
                    this->scene()->addItem(polyOb);
                    //polyOb->setPos(this->mapToScene(event->pos()));
                }
                this->clearPolyVector();
                emit polygonButtonOff();
            } else {
                QPointF *point = new QPointF(this->mapToScene(event->pos()));
                PolyDot *dot = new PolyDot(point);
                polyVector->append(dot);
                this->scene()->addItem(dot);
                this->bringToFront(dot);
            }
        }
    } else if (this->placeLine) {
        if (this->polyVector->isEmpty()) {
            QPointF *point = new QPointF(this->mapToScene(event->pos()));
            PolyDot *dot = new PolyDot(point);
            polyVector->append(dot);
            this->scene()->addItem(dot);
            this->bringToFront(dot);


        } else {
            PolyDot *item = dynamic_cast<PolyDot*>(itemAt(event->pos()));
            if (item == nullptr || item != this->polyVector->first()) {
                QPointF *point = new QPointF(this->mapToScene(event->pos()));
                PolyDot *dot = new PolyDot(point);
                polyVector->append(dot);
                this->scene()->addItem(dot);
                this->bringToFront(dot);

                QVector<QPointF*> *pointVector = new QVector<QPointF*>();
                foreach(PolyDot *dot, *this->polyVector) {
                    pointVector->append(dot->point);
                }
                LineObstacle *polyOb = new LineObstacle(pointVector);
                this->scene()->addItem(polyOb);

                this->clearPolyVector();
                emit lineButtonOff();
            }
        }
    } else {
        QGraphicsView::mousePressEvent(event);
    }


}

void View::clearPolyVector() {
    while(!polyVector->isEmpty()) {
        QGraphicsItem *item = polyVector->takeFirst();
        if (this->scene()->items().contains(item)) {
            this->scene()->removeItem(item);
            item->~QGraphicsItem();
        }
    }
}

void View::circleOn() {
    this->placeCircle = true;
    this->deselectAll();
    emit polygonButtonOff();
    emit eraserButtonOff();
    emit lineButtonOff();
}

void View::polygonOn() {
    this->placePolygon = true;
    this->deselectAll();
    emit circleButtonOff();
    emit eraserButtonOff();
    emit lineButtonOff();
}

void View::lineOn() {
    this->placeLine = true;
    this->deselectAll();
    emit polygonButtonOff();
    emit circleButtonOff();
    emit eraserButtonOff();
}

void View::eraserOn() {
    this->eraserMode = true;
    this->deselectAll();
    emit polygonButtonOff();
    emit circleButtonOff();
    emit lineButtonOff();
}

void View::circleOff() {
    this->placeCircle = false;
}

void View::polygonOff() {
    this->placePolygon = false;
    this->clearPolyVector();
}

void View::eraserOff() {
    this->eraserMode = false;
}

void View::lineOff() {
    this->placeLine = false;
    this->clearPolyVector();
}

void View::deselectAll() {
    foreach(QGraphicsItem *item, this->scene()->selectedItems()) {
        item->setSelected(false);
    }
}

void View::bringToFront(QGraphicsItem* item) {
    QList<QGraphicsItem *> overlapItems = item->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *overlaps, overlapItems) {
        if (overlaps->zValue() >= zValue) {
            zValue = item->zValue() + 0.1;
        }
    }
    item->setZValue(zValue);
}



