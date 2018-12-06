#include "startvector.h"

#include <qmath.h>

StartVector::StartVector(QVector<QPointF*> *pointVector, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsScenePositionChanges);
    this->pointVector = pointVector;
    this->pointsToLine();
    this->setSelected(true);
    this->handleVector = new QVector<PolyHandle *>();
    for (int i = 1; i < this->pointVector->size(); i++) {
        handleVector->append(new PolyHandle(this->pointVector->at(i), this));
    }
}

void StartVector::pointsToLine() {
    this->setLine(QLineF(*this->pointVector->first(), *this->pointVector->last()));
}

void StartVector::showHandles() {
    foreach(PolyHandle *handle, *this->handleVector) {
        handle->show();
    }
}

void StartVector::hideHandles() {
    foreach(PolyHandle *handle, *this->handleVector) {
        handle->hide();
    }
}

void StartVector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    this->pointsToLine();
    QBrush brush(Qt::blue);
    QPen pen(Qt::blue);

    if (this->isSelected()) {
        this->showHandles();
    } else {
        this->hideHandles();
    }

    painter->setBrush(brush);
    pen.setWidth(4);
    painter->setPen(pen);

    double angle = std::atan2(line().dy(), -line().dx());
    qreal arrowSize = 10;
    QPointF arrowP3 = this->line().p2();
    QPointF arrowP1 = arrowP3 + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                        cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = arrowP3 + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                        cos(angle + M_PI - M_PI / 3) * arrowSize);


    QPolygonF arrowHead;
    arrowHead << arrowP3 << arrowP1 << arrowP2;


    painter->drawLine(this->line());

    painter->drawPolygon(arrowHead);

    this->scene()->update();
}

QVariant StartVector::itemChange(GraphicsItemChange change, const QVariant &value)
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
