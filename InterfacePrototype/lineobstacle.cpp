#include "lineobstacle.h"


LineObstacle::LineObstacle(QVector<QPointF*> *pointVector, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    this->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);
    this->pointVector = pointVector;
    this->pointsToLine();
    this->setSelected(true);
    this->handleVector = new QVector<PolyHandle *>();
    for(int i = 0; i < this->pointVector->size(); i++) {
        handleVector->append(new PolyHandle(this->pointVector->at(i), this));
    }
}

void LineObstacle::pointsToLine() {
    this->setLine(QLineF(*this->pointVector->first(), *this->pointVector->last()));
}

void LineObstacle::showHandles() {
    foreach(PolyHandle *handle, *this->handleVector) {
        handle->show();
    }
}

void LineObstacle::hideHandles() {
    foreach(PolyHandle *handle, *this->handleVector) {
        handle->hide();
    }
}

void LineObstacle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    this->pointsToLine();
    QColor fill = Qt::gray;
    fill.setAlpha(200);
    QBrush brush(fill);
    QPen pen(Qt::black);

    if(this->isSelected()) {
        pen.setWidth(4);
        this->showHandles();
    } else {
        pen.setWidth(2);
        this->hideHandles();
    }

    painter->setBrush(brush);
    painter->setPen(Qt::transparent);

    QPointF point(this->line().center());
    qreal size = this->scene()->sceneRect().width();
    point.setX(point.x() - size);
    point.setY(point.y() - size);
    int angle = this->line().angle();
    QRectF rect(point, QSizeF(size*2, size*2));
    painter->drawPie(rect, (angle) * 16, 180*16);

    painter->setPen(pen);
    painter->drawLine(this->line());
    this->scene()->update();
//    painter->setBrush(Qt::transparent);
//    painter->setPen(QPen(Qt::red));
//    painter->drawRect(this->boundingRect());
//    painter->drawEllipse(this->scenePos(), 10, 10);
}

QVariant LineObstacle::itemChange(GraphicsItemChange change, const QVariant &value)
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
