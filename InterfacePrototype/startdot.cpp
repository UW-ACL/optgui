#include "startdot.h"


StartDot::StartDot(QPointF *point, QColor color, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
{
    this->radius = 7;
    this->point = point;
    this->setPos(*this->point);
    this->resize = false;
    this->color = color;
}

QRectF StartDot::boundingRect() const
{
    return QRectF(-this->radius, -this->radius, this->radius*2, this->radius*2);
}

void StartDot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    this->setRect(this->boundingRect());
    QRectF rec = this->boundingRect();
    QBrush brush(this->color);
    QPen pen(Qt::black);

    painter->setBrush(brush);
    painter->setPen(pen);

    painter->drawEllipse(rec);
}

void StartDot::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        this->resize = true;
    }
}

void StartDot::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (resize) {
        // TODO: act like pos is 0, 0
        qreal xPos = mouseEvent->scenePos().x(); //- parentItem()->scenePos().x();
        qreal yPos = mouseEvent->scenePos().y(); //- parentItem()->scenePos().y();
        this->point->setX(xPos);
        this->point->setY(yPos);
        this->setPos(QPointF(xPos, yPos));
        //this->update();
        this->scene()->update();
    }
}

void StartDot::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        this->resize = false;
        qDebug() << "resize = true";
    }
    //QGraphicsItem::mouseReleaseEvent(mouseEvent);
}

QVariant StartDot::itemChange(GraphicsItemChange change, const QVariant &value)
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
