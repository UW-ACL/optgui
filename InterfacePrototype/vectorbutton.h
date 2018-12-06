#ifndef VECTORBUTTON_H
#define VECTORBUTTON_H


#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

class VectorButton : public QLabel
{
    Q_OBJECT

public:
    explicit VectorButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
public slots:
    void buttonOn();
    void buttonOff();
signals:
    void vectorOn();
    void vectorOff();
private:
    QPixmap *buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // VECTORBUTTON_H
