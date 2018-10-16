#ifndef POLYBUTTON_H
#define POLYBUTTON_H


#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

class PolyButton : public QLabel
{
    Q_OBJECT

public:
    explicit PolyButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
public slots:
    void circlePlaced();
    void buttonOn();
    void buttonOff();
signals:
    void polygonOn();
    void polygonOff();
private:
    QPixmap *buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // POLYBUTTON_H
