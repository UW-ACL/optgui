#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QHBoxLayout>
#include <controlbox.h>
#include <menubutton.h>
#include <QGraphicsScene>
#include <QGraphicsAnchorLayout>
#include <QObject>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <obstacle.h>

class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QGraphicsScene *scene, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    qreal scaleFactor;
private slots:
    void openMenu();
    void closeMenu();
    void setZoom();
private:
    QRectF maxRect();
    MenuButton *openButton;
    ControlBox *controls;
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // VIEW_H
