#include "canvas.h"
#include "obstacle.h"

Canvas::Canvas(QObject *parent)
{

}

void Canvas::drawBackground(QPainter *painter, const QRectF &rect) {
    QPen gridLine(QColor(0xD3, 0xD3, 0xD3, 100));
    gridLine.setWidth(2);

    QRectF bounding = this->sceneRect();

    float segment = (bounding.right() - bounding.left()) / 10;
    for (int i = bounding.left(); i < bounding.right(); i += segment) {
        painter->drawLine(i, bounding.top(), i, bounding.bottom());
    }
    for (int i = bounding.top(); i < bounding.bottom(); i += segment) {
        painter->drawLine(bounding.left(), i, bounding.right(), i);
    }
}

void Canvas::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditem")) {
        if (children().contains(event->source())) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Canvas::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditem")) {
        if (children().contains(event->source())) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Canvas::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditem")) {
        const QMimeData *mime = event->mimeData();
        QByteArray itemData = mime->data("application/x-dnditem");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString text;
        QPoint offset;
        dataStream >> text >> offset;
        Obstacle *newLabel = new Obstacle(this, 50);
        this->addItem(newLabel);
        //newLabel->move(event->pos());
        //newLabel->show();
        //newLabel->setAttribute(Qt::WA_DeleteOnClose);

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
//    } else if (event->mimeData()->hasText()) {
//        QStringList pieces = event->mimeData()->text().split(QRegularExpression(QStringLiteral("\\s+")),
//                             QString::SkipEmptyParts);
//        QPoint position = event->pos();

//        for (const QString &piece : pieces) {
//            DragLabel *newLabel = new DragLabel(piece, this);
//            newLabel->move(position);
//            newLabel->show();
//            newLabel->setAttribute(Qt::WA_DeleteOnClose);

//            position += QPoint(newLabel->width(), 0);
//        }

//        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
