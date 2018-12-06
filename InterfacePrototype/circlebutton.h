#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

class CircleButton : public QLabel
{
    Q_OBJECT

public:
    explicit CircleButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
public slots:
    void buttonOn();
    void buttonOff();
signals:
    void circleOn();
    void circleOff();
private:
    QPixmap *buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // CIRCLEBUTTON_H