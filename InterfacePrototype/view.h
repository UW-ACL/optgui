#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QHBoxLayout>
#include "controlbox.h"
#include "menubutton.h"
#include <QGraphicsScene>
#include <QObject>
#include "obstacle.h"
#include "polydot.h"
#include "polyobstacle.h"
#include "lineobstacle.h"

class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QGraphicsScene *scene, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    qreal scaleFactor;
    void deselectAll();
public slots:
    void clearPolyVector();
    void circleOn();
    void polygonOn();
    void eraserOn();
    void circleOff();
    void polygonOff();
    void eraserOff();
    void lineOn();
    void lineOff();
signals:
    void circleButtonOff();
    void polygonButtonOff();
    void eraserButtonOff();
    void lineButtonOff();
private slots:
    void openMenu();
    void closeMenu();
    void setZoom();
    void eraseItem();
private:
    MenuButton *openButton;
    ControlBox *controls;
    bool placeCircle;
    bool eraserMode;
    bool placePolygon;
    bool placeLine;
    QVector<PolyDot*> *polyVector;

protected:
    void mousePressEvent(QMouseEvent *event);
    void bringToFront(QGraphicsItem *item);
};

#endif // VIEW_H
