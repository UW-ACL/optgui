#include "controlbox.h"
#include "createbutton.h"

#include <QLabel>
#include <QMimeData>
#include <QPainter>
#include <QtDebug>

ControlBox::ControlBox(QObject *parent)
{
//    QColor temp = this->palette().color(QPalette::Window);
//    temp.setAlpha(0);
//    QPalette tempPal = this->palette();
//    tempPal.setColor(QPalette::Window, temp);
//    this->setPalette(tempPal);
    //this->setAttribute(Qt::WA_TranslucentBackground, true);
    //this->setStyleSheet("background: transparent;");
    //this->setWindowOpacity(0.0);


    QWidget *button = new CreateButton(this);
    this->addWidget(button);



    //this->toggleViewAction();
    this->setAcceptDrops(true);
}

//void ControlBox::dragEnterEvent(QDragEnterEvent *event)
//{
//    if (event->mimeData()->hasFormat("application/x-dnditem")) {
//        if (children().contains(event->source())) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
//    } else if (event->mimeData()->hasText()) {
//        event->acceptProposedAction();
//    } else {
//        event->ignore();
//    }
//}

//void ControlBox::dragMoveEvent(QDragMoveEvent *event)
//{
//    if (event->mimeData()->hasFormat("application/x-dnditem")) {
//        if (children().contains(event->source())) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
//    } else if (event->mimeData()->hasText()) {
//        event->acceptProposedAction();
//    } else {
//        event->ignore();
//    }
//}

//void ControlBox::dropEvent(QDropEvent *event)
//{
//    if (event->mimeData()->hasFormat("application/x-dnditem")) {
//        const QMimeData *mime = event->mimeData();
//        QByteArray itemData = mime->data("application/x-dnditem");
//        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//        QString text;
//        QPoint offset;
//        dataStream >> text >> offset;
//        CreateButton *newLabel = new CreateButton(this);
//        newLabel->move(event->pos());
//        newLabel->show();
//        newLabel->setAttribute(Qt::WA_DeleteOnClose);

//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
////    } else if (event->mimeData()->hasText()) {
////        QStringList pieces = event->mimeData()->text().split(QRegularExpression(QStringLiteral("\\s+")),
////                             QString::SkipEmptyParts);
////        QPoint position = event->pos();

////        for (const QString &piece : pieces) {
////            DragLabel *newLabel = new DragLabel(piece, this);
////            newLabel->move(position);
////            newLabel->show();
////            newLabel->setAttribute(Qt::WA_DeleteOnClose);

////            position += QPoint(newLabel->width(), 0);
////        }

////        event->acceptProposedAction();
//    } else {
//        event->ignore();
//    }
//}

//void ControlBox::mousePressEvent(QMouseEvent *event)
//{
//    CreateButton *child = static_cast<CreateButton*>(childAt(event->pos()));
//    if (!child)
//        qDebug() << "not grabbing button";
//        return;

//    qDebug() << "button grabbed";

//    QPoint hotSpot = event->pos() - child->pos();

//    QByteArray itemData;
//    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
//    dataStream << "testtest" << QPoint(hotSpot);

//    QMimeData *mimeData = new QMimeData;
//    mimeData->setData("application/x-dnditem", itemData);

//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);

//    // =================================

//    QImage image(25, 25, QImage::Format_ARGB32_Premultiplied);
//    image.fill(qRgba(0, 0, 0, 0));

//    QPainter painter;
//    painter.begin(&image);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setBrush(Qt::red);
//    painter.drawRect(image.rect());
//    painter.end();

//    // =================================

//    drag->setPixmap(QPixmap::fromImage(image));
//    drag->setHotSpot(hotSpot);

//    //child->hide();

//    if (drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::CopyAction) == Qt::MoveAction)
//        child->close();
//    else
//        child->show();
//}
