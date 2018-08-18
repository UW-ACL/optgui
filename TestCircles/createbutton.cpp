#include "createbutton.h"

#include <QLabel>
#include <QMimeData>
#include <QPainter>

#include <QtDebug>

CreateButton::CreateButton(QObject *parent)
{
    this->setText("TEstTest");
}

void CreateButton::mousePressEvent(QMouseEvent *event)
{
//    CreateButton *child = static_cast<CreateButton*>(childAt(event->pos()));
//    if (!child)
//        qDebug() << "not grabbing button in child";
//        return;

    qDebug() << "button grabbed by child";

    QPoint hotSpot = event->pos() - this->pos();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << "testtest" << QPoint(hotSpot);

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditem", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    // =================================

    QImage image(25, 25, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::red);
    painter.drawRect(image.rect());
    painter.end();

    // =================================

    drag->setPixmap(QPixmap::fromImage(image));
    drag->setHotSpot(hotSpot);

    //child->hide();

    if (drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction) == Qt::MoveAction)
        this->close();
    else
        this->show();

}


//void CreateButton::dragEnterEvent(QDragEnterEvent *event)
//{

//}

//void CreateButton::dragMoveEvent(QDragMoveEvent *event)
//{

//}

//void CreateButton::dropEvent(QDropEvent *event)
//{

//}


