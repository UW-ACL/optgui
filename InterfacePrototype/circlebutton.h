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
    QPixmap getPixmap();
    void mousePressEvent(QMouseEvent *event);
public slots:
    void circlePlaced();
signals:
    void circleClicked();
private:
    QPixmap buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // CIRCLEBUTTON_H
