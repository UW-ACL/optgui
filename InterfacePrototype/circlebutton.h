#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>

class CircleButton : public QLabel
{
public:
    explicit CircleButton(QWidget *parent = nullptr);
    QPixmap getPixmap();
    void mousePressEvent(QMouseEvent *event);
private:
    QPixmap buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // CIRCLEBUTTON_H
