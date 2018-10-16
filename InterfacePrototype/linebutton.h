#ifndef LINEBUTTON_H
#define LINEBUTTON_H

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>

class LineButton : public QLabel
{
    Q_OBJECT

public:
    explicit LineButton(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
public slots:
    void buttonOn();
    void buttonOff();
signals:
    void lineOn();
    void lineOff();
private:
    QPixmap *buttonIcon;
    bool clicked;

    void generateIcon();
};

#endif // LINEBUTTON_H
