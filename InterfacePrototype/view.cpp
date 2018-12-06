#include "view.h"


View::View(QGraphicsScene *scene, QWidget * parent)
    : QGraphicsView(scene, parent)
{
    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    this->setRenderHint(QPainter::Antialiasing);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

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

    connect(this->controls->controlPanel->startButton, SIGNAL(startOn()), this, SLOT(startOn()), Qt::DirectConnection); // start mode on
    connect(this->controls->controlPanel->startButton, SIGNAL(startOff()), this, SLOT(startOff()), Qt::DirectConnection); // start mode off
    connect(this, SIGNAL(startButtonOff()), this->controls->controlPanel->startButton, SLOT(buttonOff()), Qt::DirectConnection); // start button off

    connect(this->controls->controlPanel->vectorButton, SIGNAL(vectorOn()), this, SLOT(vectorOn()), Qt::DirectConnection); // vector mode on
    connect(this->controls->controlPanel->vectorButton, SIGNAL(vectorOff()), this, SLOT(vectorOff()), Qt::DirectConnection); // vector mode off
    connect(this, SIGNAL(vectorButtonOff()), this->controls->controlPanel->vectorButton, SLOT(buttonOff()), Qt::DirectConnection); // vector button off

    connect(this->scene(), SIGNAL(selectionChanged()), this, SLOT(eraseItem())); // eraser item

    this->polyVector = new QVector<PolyDot*>();
    this->placePolygon = false;
    this->placeLine = false;
    this->placeStart = false;
    this->placeVector = false;

    this->startDot = nullptr;
    this->endDot = nullptr;
    this->startVector = nullptr;
}

void View::openMenu() {
    this->openButton->hide();
    this->controls->show();
    repaint();
}

void View::closeMenu() {
    this->controls->hide();
    this->openButton->show();
    this->scene()->update();
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
    } else if (this->placeStart) {
        if (this->startDot == nullptr) {
            QPointF *point = new QPointF(this->mapToScene(event->pos()));
            this->startDot = new StartDot(point, Qt::green);
            this->scene()->addItem(this->startDot);
            this->bringToFront(this->startDot);
        } else {
            StartDot *item = dynamic_cast<StartDot*>(itemAt(event->pos()));
            if (item == nullptr || item != this->startDot) {
                QPointF *point = new QPointF(this->mapToScene(event->pos()));
                this->endDot = new StartDot(point, Qt::red);
                this->scene()->addItem(this->endDot);
                this->bringToFront(this->endDot);

                emit startButtonOff();
            }
        }
    } else if (this->placeVector) {
        if (this->startDot != nullptr) {
            QPointF *point = new QPointF(this->mapToScene(event->pos()));
            QVector<QPointF*> *pointVector = new QVector<QPointF*>();
            pointVector->append(this->startDot->point);
            pointVector->append(point);

            this->startVector = new StartVector(pointVector);
            this->scene()->addItem(this->startVector);
            this->bringToFront(this->startVector);

            emit vectorButtonOff();
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
        }
    }
}

void View::circleOn() {
    this->placeCircle = true;
    this->deselectAll();

    if (this->placePolygon) { emit polygonButtonOff(); }
    if (this->eraserMode) { emit eraserButtonOff(); }
    if (this->placeLine) { emit lineButtonOff(); }
    if (this->placeStart) { emit startButtonOff(); }
    if (this->placeVector) { emit vectorButtonOff(); }
}

void View::polygonOn() {
    this->placePolygon = true;
    this->deselectAll();

    if (this->placeCircle) { emit circleButtonOff(); }
    if (this->eraserMode) { emit eraserButtonOff(); }
    if (this->placeLine) { emit lineButtonOff(); }
    if (this->placeStart) { emit startButtonOff(); }
    if (this->placeVector) { emit vectorButtonOff(); }
}

void View::lineOn() {
    this->placeLine = true;
    this->deselectAll();

    if (this->placePolygon) { emit polygonButtonOff(); }
    if (this->eraserMode) { emit eraserButtonOff(); }
    if (this->placeCircle) { emit circleButtonOff(); }
    if (this->placeStart) { emit startButtonOff(); }
    if (this->placeVector) { emit vectorButtonOff(); }
}

void View::eraserOn() {
    this->eraserMode = true;
    this->deselectAll();

    if (this->placePolygon) { emit polygonButtonOff(); }
    if (this->placeCircle) { emit circleButtonOff(); }
    if (this->placeLine) { emit lineButtonOff(); }
    if (this->placeStart) { emit startButtonOff(); }
    if (this->placeVector) { emit vectorButtonOff(); }
}

void View::startOn() {
    this->placeStart = true;
    this->deselectAll();

    if (this->startDot) {
        this->scene()->removeItem(this->startDot);
        this->startDot = nullptr;
    }

    if (this->endDot) {
        this->scene()->removeItem(this->endDot);
        this->endDot = nullptr;
    }

    if (this->startVector) {
        this->scene()->removeItem(this->startVector);
        this->startVector = nullptr;
    }

    if (this->placePolygon) { emit polygonButtonOff(); }
    if (this->eraserMode) { emit eraserButtonOff(); }
    if (this->placeLine) { emit lineButtonOff(); }
    if (this->placeCircle) { emit circleButtonOff(); }
    if (this->placeVector) { emit vectorButtonOff(); }
}

void View::vectorOn() {
    this->placeVector = true;
    this->deselectAll();

    if (this->startVector) {
        this->scene()->removeItem(this->startVector);
        this->startVector = nullptr;
    }

    if (this->placePolygon) { emit polygonButtonOff(); }
    if (this->eraserMode) { emit eraserButtonOff(); }
    if (this->placeLine) { emit lineButtonOff(); }
    if (this->placeStart) { emit startButtonOff(); }
    if (this->placeCircle) { emit circleButtonOff(); }
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

void View::vectorOff() {
    this->placeVector = false;
}

void View::startOff() {
    this->placeStart = false;
    if (this->endDot == nullptr) {
        this->scene()->removeItem(this->startDot);
        this->startDot = nullptr;
    }
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



