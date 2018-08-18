#ifndef VIEW_H
#define VIEW_H

#include <QPainter>
#include <QGraphicsView>

class View : public QGraphicsView
{
public:
    View(QWidget * parent);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool compareRect(QRectF rect1, QRectF rect2);
    void zoom();
};

#endif // VIEW_H
