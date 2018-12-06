#include "polyobstacle.h"

PolyObstacle::PolyObstacle(QVector<QPointF*> *pointVector, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);
    this->pointVector = pointVector;
    this->setPoly();
    this->setSelected(true);
    this->handleVector = new QVector<PolyHandle *>();
    for(int i = 1; i < this->pointVector->size(); i++) {
        handleVector->append(new PolyHandle(this->pointVector->at(i), this));
    }
}

QRectF PolyObstacle::boundingRect() const
{
    return this->polygon().boundingRect();
}

void PolyObstacle::setPoly() {
    QPolygonF poly;
    foreach(QPointF *point, *this->pointVector) {
        poly << *point;
    }
    this->setPolygon(poly);
}

void PolyObstacle::showHandles() {
    foreach(PolyHandle *handle, *this->handleVector) {
        handle->show();
    }
}

void PolyObstacle::hideHandles() {
    foreach(PolyHandle *handle, *this->handleVector) {
        handle->hide();
    }
}

void PolyObstacle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    this->setPoly();
    QColor fill = Qt::gray;
    fill.setAlpha(200);
    QBrush brush(fill);
    QPen pen(Qt::black);

    if(this->isSelected()) {
        pen.setWidth(3);
        this->showHandles();
    } else {
        pen.setWidth(1);
        this->hideHandles();
    }

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawPolygon(this->polygon(), Qt::OddEvenFill);
    this->scene()->update();
//    painter->setBrush(Qt::transparent);
//    painter->setPen(QPen(Qt::red));
//    painter->drawRect(this->boundingRect());
//    painter->drawEllipse(this->scenePos(), 10, 10);
}

QVariant PolyObstacle::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        // QPointF newPos = value.toPointF();
        QRectF itemRect = this->sceneBoundingRect();
        QRectF rect = scene()->sceneRect();

        if (!rect.contains(itemRect)) {
            this->scene()->setSceneRect((this->scene()->sceneRect()).united(itemRect));
            this->scene()->update();
            this->scene()->views().first()->setSceneRect(this->scene()->sceneRect());
        }
    }
    return QGraphicsItem::itemChange(change, value);
}
