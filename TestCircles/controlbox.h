#ifndef CONTROLBOX_H
#define CONTROLBOX_H

#include <QMouseEvent>
#include <QToolBar>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

class ControlBox : public QToolBar
{
public:
    ControlBox(QObject *parent);

protected:
//    void mousePressEvent(QMouseEvent *event);
//    void dragEnterEvent(QDragEnterEvent *event) override;
//    void dragMoveEvent(QDragMoveEvent *event) override;
//    void dropEvent(QDropEvent *event) override;
};

#endif // CONTROLBOX_H
