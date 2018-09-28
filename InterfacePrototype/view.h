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
signals:
    void circlePlaced();
private slots:
    void circleClicked();
    void openMenu();
    void closeMenu();
    void setZoom();
private:
    MenuButton *openButton;
    ControlBox *controls;
    bool placeCircle;
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // VIEW_H
