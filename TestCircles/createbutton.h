#ifndef CREATEBUTTON_H
#define CREATEBUTTON_H

#include <QPushButton>

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDrag>
#include <QLabel>

class CreateButton : public QLabel
{
public:
    CreateButton(QObject *parent);

protected:
//    void dragEnterEvent(QDragEnterEvent *event) override;
//    void dragMoveEvent(QDragMoveEvent *event) override;
//    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event);
};

#endif // CREATEBUTTON_H
